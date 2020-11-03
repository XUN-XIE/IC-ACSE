test = {
  'name': 'question 3.10',
  'points': 1,
  'suites': [
    {
      'cases': [
        {
          'code': r"""
          >>> a.shape
          (100, 2)
          """,
          'hidden': False,
          'locked': False
        }
      ],
      'scored': True,
      'setup': 'file_array = a = np.loadtxt("ex8_out.dat", usecols=(2, 5))',
      'teardown': '',
      'type': 'doctest'
    },
    {
      'cases': [
        {
          'code': r"""
          >>> np.array_equal(a[0], [-4., 0.])
          True
          """,
          'hidden': False,
          'locked': False
        },
        {
          'code': r"""
          >>> np.array_equal(a[50], [0.04, 0.4])
          True
          """,
          'hidden': False,
          'locked': False
        },
        {
          'code': r"""
          >>> np.array_equal(a[-1], [4., 0.])
          True
          """,
          'hidden': False,
          'locked': False
        }
      ],
      'scored': True,
      'setup': 'file_array = a = np.loadtxt("ex8_out.dat", usecols=(2, 5))',
      'teardown': '',
      'type': 'doctest'
    }
  ]
}
