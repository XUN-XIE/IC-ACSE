import unittest
import foo

class TestSolveQuadratic(unittest.TestCase):
    def test_solve_quadratic(self):
    
        self.assertEqual(foo.solve_quadratic(1,0,-1), (-1.0,1.0))
        self.assertEqual(foo.solve_quadratic(1,0,0), 0.0 )
        self.assertEqual(foo.solve_quadratic(1,0,1), None)
