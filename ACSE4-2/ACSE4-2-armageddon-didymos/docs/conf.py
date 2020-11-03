import sys
import os

sys.path.insert(0, os.path.abspath(os.sep.join((os.curdir,'..'))))

project = 'Armageddon'
extensions = ['sphinx.ext.autodoc',
              'sphinx.ext.napoleon']
source_suffix = '.rst'
master_doc = 'index'
exclude_patterns = ['_build']
autoclass_content = "both"
