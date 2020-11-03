test = {
  'name': 'question 1.16',
  'points': 1,
  'suites': [
    {
      'cases': [
        {
          'code': r"""
          >>> type(distances_list)==list
          True
          """,
          'hidden': False,
          'locked': False
        },
      ],
      'scored': True,
      'setup': 'distances_list=distance(0.1, 0.6, 6)',
      'teardown': '',
      'type': 'doctest'
    },
    {
      'cases': [
        {
          'code': r"""
          >>> np.allclose(distance(0.1, 0.6, 6), [0.5019, 0.770275, 0.9024000000000001, 0.8982749999999997, 0.7578999999999998, 0.4812750000000001])
          True
          """,
          'hidden': False,
          'locked': False
        },
        {
          'code': r"""
          >>> np.allclose(distance(1, 6, 5, 3), [-6.8100000000000005, -33.24, -79.29, -144.96, -230.25])
          True
          """,
          'hidden': False,
          'locked': False
        }
      ],
      'scored': True,
      'setup': 'import numpy as np',
      'teardown': '',
      'type': 'doctest'
    }
  ]
}
