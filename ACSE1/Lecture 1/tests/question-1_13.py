test = {
  'name': 'question 1.13',
  'points': 1,
  'suites': [
    {
      'cases': [
        {
          'code': r"""
          >>> type(odd_list)==list
          True
          """,
          'hidden': False,
          'locked': False
        },
        {
          'code': r"""
          >>> np.allclose(odd_list, [1, 3, 5, 7, 9])
          True
          """,
          'hidden': False,
          'locked': False
        },
      ],
      'scored': True,
      'setup': 'import numpy as np',
      'teardown': '',
      'type': 'doctest'
    }
  ]
}
