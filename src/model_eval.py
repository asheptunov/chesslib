import os
import sys
import csv
import time
import torch
import numpy as np
import ctypes
import multiprocessing
import subprocess
from progress.bar import Bar

from . import chess
from . import eval_func_convo_net

STARTING_FEN = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -'

shift = 4 * np.arange(8)
masks = np.array([0xf], dtype=np.uint32) << shift

def board_tensor(board, one_hot=False, free=False):
    '''
    Returns an (8,8) shape torch.tensor form of the board's ranks array
    Frees the board if specified
    '''
    t = torch.tensor((np.array(board.contents.ranks, dtype=np.uint32).reshape(-1, 1) & masks) >> shift)
    if (one_hot):
        t = torch.eye(13)[t]  # 13 = 6 white + 6 black + 1 empty
    if (free):
        chess.board_free(board)
    return t

def write_snapshots(snapshots, outcomes, turns, snapshot_file, append=False):
    '''
    Writes a list of snapshots to the snapshot_file. Appends to the file if append=True, otherwise truncates
    the file first. For each snapshot, writes a comma separated row with the snapshot (board fen), outcome,
    and the turn number.
    '''
    assert len(snapshots) == len(outcomes) and len(outcomes) == len(turns)
    with open(snapshot_file, 'a' if append else 'w') as snapshot_file_:
        writer = csv.writer(snapshot_file_)
        for i in range(len(turns)):
            writer.writerow([snapshots[i], outcomes[i], turns[i]])

def play_game(white_model, black_model, max_turns: int):
    '''
    Plays a game of chess using the specified models for the white and black players. Stops playing once
    a player wins, the game ends in a draw, or max_turns have been played without either prior outcome.
    Models should be callable using model(board) and should return a legal move to apply to the board.
    Captures snapshots of every board state throughout the game.
    Returns a tuple containing a list of all board snapshots (fens) and the outcome of the game (0 if white won,
    1 if black won, 0.5 if the game ended in a draw, and -1 if the game didn't end within max_turns turns).
    '''
    # 0. initialize
    board = chess.board_make(STARTING_FEN)
    snapshots = []  # a list of snapshots (intermediate board states) throughout the game
    counts = {}
    turn = 0  # the current turn (starts with 0)
    player = 0  # 0 = white; 1 = black
    outcome = -1  # the player that won

    bar = None
    while True:
        # sys.stdout.write('process %d turn %d\n' % (os.getpid(), turn))
        # sys.stdout.flush()
        # print('process %d turn %d' % (os.getpid(), turn))
        # 1. take a snapshot
        fen = chess.board_to_fen(board)
        snapshots.append(fen)
        counts[fen] = 1 if fen not in counts else counts[fen] + 1
        # 2a. stop if the current player lost
        if chess.board_is_mate(board):
            outcome = int(not player)
            break
        # 2b. stop if stalemate / insufficient material / fifty-move rule
        if chess.board_is_stalemate(board):
            outcome = 0.5
            break
        # 2c. stop if the game is a draw by repitition
        if counts[fen] == 3:
            outcome = 0.5
            break
        # 2d. stop if we hit the max game length
        if turn >= max_turns:
            break
        # 3. get and apply the model's preferred move
        move = black_model(board) if player else white_model(board)
        chess.board_apply_move(board, move)
        chess.move_free(move)
        # 4. update bookkeeping
        turn += 1
        player = not player
    # 6. cleanup
    # print('process %d outcome %d' % (os.getpid(), outcome))
    chess.board_free(board)
    return (snapshots, outcome)

def model_eval(num_games: int, white_model, black_model, max_turns: int, num_samples_per_game: int, no_unfinished: bool, snapshot_file, snapshot_file_lock=None):
    '''
    Evaluates the given models by forcing them to play n games against each other.
    Appends up to num_samples_per_game distinct snapshots of each game to the snapshot_file, synchronizing on the snapshot_file_lock
    if specified.
    Returns the outcome totals (0 = white won, 1 = black won, 0.5 = draw, -1 = unfinished)
    '''
    if num_games < 1:
        raise ValueError('must play at least 1 game, specified %d' % n)
    i = 0
    all_snapshots = []
    all_outcomes = []
    all_turns = []
    outcome_totals = {0: 0, 1: 0, 0.5: 0, -1: 0}
    # play the games
    with Bar('Playing', max=num_games, suffix='%(percent).1f%% [%(index)d/%(max)d]  [~%(avg)ds per game] [%(eta_td)s remaining]') as bar:
        while i < num_games:
            snapshots, outcome = play_game(white_model, black_model, max_turns)
            # ignore unfinished games if specified
            if outcome == -1 and no_unfinished:
                continue
            # sample at most num_samples_per_game distinct snapshots
            turns = np.sort(np.random.choice(len(snapshots), min(num_samples_per_game, len(snapshots)), replace=False))
            snapshots = np.array(snapshots)[turns]
            # aggregate results
            all_snapshots.append(snapshots)
            all_turns.append(turns)
            all_outcomes.append(np.repeat(outcome, len(turns)))
            if outcome not in outcome_totals:
                raise ValueError('bad outcome %d' % outcome)
            outcome_totals[outcome] += 1
            i += 1
            bar.next()
    all_snapshots = np.concatenate(all_snapshots, axis=0)
    all_outcomes = np.concatenate(all_outcomes, axis=0)
    all_turns = np.concatenate(all_turns, axis=0)
    if snapshot_file_lock is not None:
        snapshot_file_lock.acquire()
    write_snapshots(all_snapshots, all_outcomes, all_turns, snapshot_file, append=True)
    if snapshot_file_lock is not None:
        snapshot_file_lock.release()
    return outcome_totals

def _model(evaluate, board):
    '''
    Wrapper for returning a move to apply from an evaluation function and a board.
    '''
    moves = chess.board_get_moves(board)
    utilities = []
    for i in range(moves.contents.len):
        move = ctypes.cast(chess.alst_get(moves, i), ctypes.POINTER(chess.MOVE))
        future_board = chess.board_copy(board)
        chess.board_apply_move(future_board, move)
        utilities.append(evaluate(board_tensor(future_board, one_hot=True, free=True).unsqueeze(0)).item())
    utilities = np.array(utilities)
    utilities = np.exp(utilities) / np.sum(np.exp(utilities))  # softmax
    j = np.random.choice(moves.contents.len, size=1, p=utilities).item()  # sample one of the moves
    move = chess.move_cpy(ctypes.cast(chess.alst_get(moves, j), ctypes.POINTER(chess.MOVE)))
    chess.alst_free(moves, ctypes.cast(chess.move_free, chess.alst_free_func_type))
    return move

class ModelEvalWorker(multiprocessing.Process):
    def __init__(self, num_games: int, model_file, max_turns: int, num_samples_per_game: int, no_unfinished: bool, snapshot_file, snapshot_file_lock):
        super(ModelEvalWorker, self).__init__()
        self.num_games = num_games
        self.max_turns = max_turns
        self.num_samples_per_game = num_samples_per_game
        self.no_unfinished = no_unfinished
        self.snapshot_file = snapshot_file
        self.snapshot_file_lock = snapshot_file_lock
        self.model = eval_func_convo_net.EvalFuncConvoNet()
        self.model.load_state_dict(torch.load(model_file))
        self.model.eval()

    def run(self):
        pid = os.getpid()
        print('process %d started' % pid)
        np.random.seed(pid)
        model = lambda board: _model(self.model, board)
        outcome_totals = model_eval(
            self.num_games,
            model,
            model,
            self.max_turns,
            self.num_samples_per_game,
            self.no_unfinished,
            self.snapshot_file,
            snapshot_file_lock=self.snapshot_file_lock)
        print('process %d finished with outcome totals %s' % (os.getpid(), outcome_totals))

if __name__ == '__main__':
    NUM_GAMES = 1_000
    NUM_WORKERS = 1
    MAX_TURNS = 100
    NUM_SAMPLES_PER_GAME = 10
    NO_UNFINISHED = True
    DATA_DIR = os.path.join(os.path.split(os.path.dirname(__file__))[0], 'data')  # ../data
    ID = hash(time.time())
    MODEL_FILE = os.path.join(DATA_DIR, 'model_%s.pt' % ID)
    SNAPSHOT_FILE = os.path.join(DATA_DIR, 'snapshots_%s.csv' % ID)
    SNAPSHOT_FILE_LOCK = multiprocessing.Lock()
    print('playing %d games with %d workers, %d max turns, %d samples per game, %s unfinished games' % (NUM_GAMES, NUM_WORKERS, MAX_TURNS, NUM_SAMPLES_PER_GAME, 'no' if NO_UNFINISHED else 'including'))
    print('loading model from %s' % MODEL_FILE)
    print('writing dataset to %s' % SNAPSHOT_FILE)

    model = eval_func_convo_net.EvalFuncConvoNet()
    torch.save(model.state_dict(), MODEL_FILE)

    workers = []
    for i in range(NUM_WORKERS):
        workers.append(ModelEvalWorker(NUM_GAMES, MODEL_FILE, MAX_TURNS, NUM_SAMPLES_PER_GAME, NO_UNFINISHED, SNAPSHOT_FILE, SNAPSHOT_FILE_LOCK))

    for worker in workers:
        worker.start()

    start_time = time.time()

    for worker in workers:
        worker.join()

    end_time = time.time()
    elapsed = end_time - start_time
    print('finished in %d m %.1f s' % (elapsed / 60, elapsed % 60))
    print('wrote dataset to %s' % SNAPSHOT_FILE)
