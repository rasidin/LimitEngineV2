import sys
import os
import xml.etree.ElementTree as et
import xml.dom.minidom as md

def printTitle():
    print('ObjToModelXML')
    print('Model OBJ file to LEModel XML file')
    print('author minseob')

def printUsage():
    print('Usage : ObjToModelXML <in> <out>')

def getArguments():
    output = {}
    for arg in sys.argv[1:]:
        if 'in' not in output:
            output['in'] = arg
        elif 'out' not in output:
            output['out'] = arg
    return output

def setXMLVector3(parentElement, vectorname, vectorvalues):
    newelem = et.SubElement(parentElement, vectorname)
    xelem = et.SubElement(newelem, 'x')
    xelem.text = vectorvalues[0]
    yelem = et.SubElement(newelem, 'y')
    yelem.text = vectorvalues[1]
    zelem = et.SubElement(newelem, 'z')
    zelem.text = vectorvalues[2]

def setXMLiVector3(parentElement, vectorname, vectorvalues):
    newelem = et.SubElement(parentElement, vectorname)
    xelem = et.SubElement(newelem, 'i0')
    xelem.text = '%d'%vectorvalues[0]
    yelem = et.SubElement(newelem, 'i1')
    yelem.text = '%d'%vectorvalues[1]
    zelem = et.SubElement(newelem, 'i2')
    zelem.text = '%d'%vectorvalues[2]

def convertObjToXML(arguments):
    inFile = open(arguments["in"], mode="r")
    objlines = inFile.readlines()
    inFile.close()

    vertices = []
    indices = []	
    texcoords = []
    normals = []
    faceset = []
    meshes = []
    for l in objlines:
        if l[0] == 'v':
            if l[1] == 'n':
                vntokens = l.replace('\n', '').replace('  ', ' ').split(' ')
                normals.append([vntokens[1], vntokens[2], vntokens[3]])
            elif l[1] == 't':
                vttoken = l.replace('\n', '').replace('  ', ' ').split(' ')
                texcoords.append([vttoken[1], vttoken[2], vttoken[3]])
            else:
                vtokens = l.replace('\n', '').replace('  ', ' ').split(' ')
                vertices.append([vtokens[1], vtokens[2], vtokens[3]])
        elif l[0] == 'f':
            ftoken = l.split(' ')
            indexset = []
            for ft in ftoken[1:]:
                fts = ft.replace('\n', '').replace('  ', ' ').split('/')
                if len(fts) == 3:
                    if len(fts[1]) == 0:
                        fts[1] = "1"
                    indexset.append([int(fts[0])-1, int(fts[1])-1, int(fts[2])-1])
                    faceindex = int(fts[0])-1
                    while len(faceset) <= faceindex:
                        faceset.append([0, 0, 0])
                    faceset[faceindex] = [int(fts[0])-1, int(fts[1])-1, int(fts[2])-1]
            polygonnum = len(indexset) - 2
            for pidx in range(polygonnum):
                indices.append([indexset[0][0], indexset[pidx+1][0], indexset[pidx+2][0]])
        elif l[0] == 'o':
            otoken = l.split(' ')
            meshes.append({"Name": otoken[1].replace('\n', ''), "BeginIndex": len(indices)})

    modelroot = et.Element('model')
    materials = et.SubElement(modelroot, 'materials')
    for m in meshes:
        material = et.SubElement(materials, 'material')
        materialID = et.SubElement(material, 'ID')
        materialID.text = m["Name"]
        materialShader = et.SubElement(material, 'SHADER')
        materialShader.text = 'standard'

    elements = et.SubElement(modelroot, 'elements')
    mesh = et.SubElement(elements, 'mesh')
    transform = et.SubElement(mesh, 'transform')
    setXMLVector3(transform, 'position', ['0.0', '0.0', '0.0'])
    setXMLVector3(transform, 'rotation', ['0.0', '0.0', '0.0'])
    setXMLVector3(transform, 'scale', ['1.0', '1.0', '1.0'])
    verticeselem = et.SubElement(mesh, 'vertices')
    for f in faceset:
        vertex = et.SubElement(verticeselem, 'vertex')
        setXMLVector3(vertex, 'position', vertices[f[0]])
        setXMLVector3(vertex, 'normal', normals[f[2]])
        setXMLVector3(vertex, 'texcoord', texcoords[f[1]])

    indexnum = 0
    meshindex = 0
    currentMesh = meshes[0]
    indiceselem = et.SubElement(mesh, 'indices')
    for i in indices:
        if meshindex + 1 < len(meshes) and indexnum >= meshes[meshindex+1]["BeginIndex"]:
            meshindex = meshindex + 1
            currentMesh = meshes[meshindex]
        index = et.SubElement(indiceselem, 'index')
        indexmaterial = et.SubElement(index, 'material')
        indexmaterial.text = currentMesh["Name"]
        setXMLiVector3(index, 'polygon', i)
        indexnum = indexnum + 1

    document = md.parseString(et.tostring(modelroot))

    outfile = open(arguments["out"], 'w')
    document.writexml(outfile, newl='\n', indent='', addindent='  ')
    outfile.close()

def main():
    printTitle()

    args = getArguments()

    if len(args) < 2:
        printUsage()

    convertObjToXML(args)

if __name__ == '__main__':
    main()