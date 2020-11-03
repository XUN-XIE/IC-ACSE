import vtk
import numpy as np

int size = 10

def test_file_writer_output():
    reader = vtk.vtkXMLPolyDataReader()
    reader.SetFileName('tests/test_file_writer.vtp')
    reader.Update()

    pdata = reader.GetOutput()
    assert pdata.GetNumberOfCells() == size #total number of points
    assert pdata.GetNumberOfPoints() ==  size

#velocity check
def test_boundaries():
    reader = vtk.vtkXMLPolyDataReader()
    reader.SetFileName('tests/test_file_writer.vtp')
    reader.Update()

    pdata = reader.GetOutput()
    for i in range(pdata.GetNumberOfPoints()):
            v = 0
            for j in range(2):
                    assert (pdate.GetPoint(i)[j] >= 0 and pdata.GetPoint(i)[j] < size)
                    v += pdate.GetPointData().GetArray('Velocity').GetTuple(i)[j] ** 2
                assert np.sqrt(v) < 20


def test_velocity():
    reader = vtk.vtkXMLPolyDataReader()
    reader.SetFileName('tests/test_file_writer.vtp')
    reader.Update()

    pdata = reader.GetOutput()
    for i in range(pdata.GetNumberOfPoints()):
            v = 0
            for j in range(2):
                    assert (pdate.GetPoint(i)[j] >= 0 and pdata.GetPoint(i)[j] < size)
                    v += pdate.GetPointData().GetArray('Velocity').GetTuple(i)[j] ** 2
                assert v < 1498

def test_pressure():
    reader = vtk.vtkXMLPolyDataReader()
    reader.SetFileName('tests/test_file_writer.vtp')
    reader.Update()

    pdata = reader.GetOutput()
    assert (pdata.GetPointData().GetArray('Pressure').GetValue(5)
            == 5.0)
