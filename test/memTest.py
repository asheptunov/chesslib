import unittest
import os
import subprocess

class TestMemoryLeaks(unittest.TestCase):
    def leak_check(self, binary):
        valgrind_log = os.path.join(os.path.split(os.path.dirname(__file__))[0], 'log', ('valgrind_%s' % os.path.split(binary)[1]))
        valgrind_args = [
            'valgrind',
            '--error-exitcode=1',
            '--leak-check=full',
            '--show-leak-kinds=all',
            ('--log-file=%s' % valgrind_log),
            binary]
        valgrind_ret = subprocess.call(valgrind_args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        self.assertFalse(valgrind_ret, msg=('valgrind detected leaks; refer to log at %s' % valgrind_log))

    def test_arraylist_memory_leaks(self):
        self.leak_check(os.path.join(os.path.split(os.path.dirname(__file__))[0], 'bin', 'test', 'arraylistTest'))

    def test_board_memory_leaks(self):
        self.leak_check(os.path.join(os.path.split(os.path.dirname(__file__))[0], 'bin', 'test','boardTest'))
    
    def test_move_memory_leaks(self):
        self.leak_check(os.path.join(os.path.split(os.path.dirname(__file__))[0], 'bin', 'test', 'moveTest'))
    
    def test_movegen_memory_leaks(self):
        self.leak_check(os.path.join(os.path.split(os.path.dirname(__file__))[0], 'bin', 'test', 'movegenTest'))

    # def test_perft_memory_leaks(self):
    #     self.leak_check(os.path.join(os.path.split(os.path.dirname(__file__))[0], 'bin', 'test', 'perftTest'))

if __name__ == '__main__':
    unittest.main()
