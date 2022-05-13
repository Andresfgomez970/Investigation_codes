from operator import ge
import unittest
from read_data import get_data


class TestSum(unittest.TestCase):

    def test_get_data(self):
        print(get_data("../data/defaults/DefaultLauraUniform.dat"))
        #  self.assertEqual(sum([1, 2, 3]), 6, "Should be 6")



if __name__ == '__main__':
    unittest.main()
