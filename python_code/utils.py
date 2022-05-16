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