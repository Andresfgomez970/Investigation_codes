import numpy as np

def is_int(value):
  try:
    int(value)
    return True
  except:
    return False

def is_float(value):
  try:
    float(value)
    return True
  except:
    return False


def is_number(value):
  return is_float(value) or is_int(value)


def smaller_divisor_and_remainder(x):
    divisor = 2
    while divisor <= x:
        if x % divisor == 0:
            return divisor, int(x / divisor)
        divisor += 1
    return 1, x


def arr(x):
  return np.array(x)