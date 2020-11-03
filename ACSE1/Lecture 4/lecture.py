from client.api.notebook import Notebook
from client.api import assignment
from client.utils import auth

args = assignment.Settings(server='clewolffautook21.eastus.cloudapp.azure.com/okpy')
ok = Notebook('./lecture.ok', args)
