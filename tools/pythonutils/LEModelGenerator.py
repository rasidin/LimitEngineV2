import sys
import os
import math

def printTitle():
	print("LEModelGenerator ver 0.1")
	print("coded by minseob")

def printUsage():
	print("LEModelGenerator [command] <options...> [inputfile] [outputfile]")
	print("Comamnds: makesphere")

def getArguments(argv):
	output = {"options":[]}
	
	for arg in argv[1:]:
		if arg[0] == '-':
			output["options"].append(arg)
		elif "command" not in output:
			output["command"] = arg
		elif "input" not in output:
			output["input"] = arg
		elif "output" not in output:
			output["output"] = arg
	
	return output

def makeLEText_Stride(stride):
	out = ''
	for i in range(stride + 1):
		out = out + ' '
	return out

def makeLEText_List(stride, l, out):
	for n, item in l.items():
		stride_word = ''
		if stride > 0:
			stride_word = makeLEText_Stride(stride)
		if type(item) is dict:
			out.write(stride_word + n + ' {\n')
			makeLEText_List(stride + 4, item, out)
			out.write(stride_word + '}\n')
		elif type(item) is list:
			out.write(stride_word + n + ' {\n')
			for sub in item:
				out.write(stride_word + '    {\n')
				makeLEText_List(stride + 8, sub, out)
				out.write(stride_word + '    }\n')
			out.write(stride_word + '}\n')
		else:
			out.write(stride_word + n + ' ' + item + '\n')

def makeLEText(data, out):
	makeLEText_List(0, data, out)

def makeSphere(arguments):
	outFile = open(arguments["input"], "w")

	splitcount = 32
	vertices = []
	for t in range(splitcount):
		theta = float(t) / (splitcount - 1) * math.pi + math.pi * 0.5
		sinxy = abs(math.sin(float(t) / (splitcount - 1) * math.pi))
		y = math.sin(theta)
		if t > 0 and t < splitcount-1:
			for p in range(splitcount):
				delta = float(p) / splitcount * 2.0 * math.pi
				x = math.sin(delta) * sinxy
				z = math.cos(delta) * sinxy
				tu = x
				tv = 0.5 + theta * -0.5
				tx =-z
				ty = 0
				tz = x
				bx =-tz * y
				by = tz * x - tx * z
				bz =-tx * y
				vertices.append({"POSITION" : "[%f %f %f]"%(x, y, z), "NORMAL" : "[%f %f %f]"%(x, y, z), "TANGENT" : "[%f %f %f]"%(tx, ty, tz), "BINORMAL" : "[%f %f %f]"%(bx, by, bz), "TEXCOORD" : "[%f %f]"%(tu, tv), "COLOR" : "[255 255 255 255]"})
		else:
			x = 0
			z = 0
			tu = 0
			tv = 0.5 + theta * -0.5
			tx = y
			ty = 0
			tz = 0
			bx = 0
			by = 0
			bz =-y
			vertices.append({"POSITION" : "[%f %f %f]"%(x, y, z), "NORMAL" : "[%f %f %f]"%(x, y, z), "TANGENT" : "[%f %f %f]"%(tx, ty, tz), "BINORMAL" : "[%f %f %f]"%(bx, by, bz), "TEXCOORD" : "[%f %f]"%(tu, tv), "COLOR" : "[255 255 255 255]"})

	print(len(vertices))

	indices = []
	for t in range(splitcount-1):
		if t == 0:
			for p in range(splitcount):
				i0 = 0
				i1 = p + 1
				i2 = (p + 1) % (splitcount) + 1
				indices.append({"MATERIAL" : "Standard", "POLYGON" : "[%d %d %d]"%(i0, i1, i2)})
		elif t == splitcount - 2:
			for p in range(splitcount):
				i0 = p + (t-1) * (splitcount) + 1
				i1 =(t) *(splitcount) + 1
				i2 =(p + 1)%splitcount + (t-1) * (splitcount) + 1
				indices.append({"MATERIAL" : "Standard", "POLYGON" : "[%d %d %d]"%(i0, i1, i2)})
		else:
			for p in range(splitcount):
				i0 = (t - 1) * splitcount + 1 + p
				i1 = (t    ) * splitcount + 1 + p
				i2 = (t - 1) * splitcount + 1 +(p + 1) % splitcount
				indices.append({"MATERIAL" : "Standard", "POLYGON" : "[%d %d %d]"%(i0, i1, i2)})
				i3 = i1
				i4 = t * splitcount + 1 + (p + 1) % splitcount
				i5 = i2
				indices.append({"MATERIAL" : "Standard", "POLYGON" : "[%d %d %d]"%(i3, i4, i5)})

	data = {
		"FILETYPE" : "MODEL",
		"NAME" : "SPHERE",
		"DATA" : {
			"MATERIALS" : {
				"ID" : "Standard",
				"SHADER" : "Standard"
			},
			"ELEMENTS" : {
				"MESH" : {
					"TRANSFORM" : {
						"POSITION" : "[0.0 0.0 0.0]",
						"SCALE" : "[1.0 1.0 1.0]",
						"ROTATION" : "[0.0 0.0 0.0]"
					},
					"VERTICES" : vertices,
					"INDICES" : indices
				}
			}
		}
	}
	
	makeLEText(data, outFile)
		
	outFile.close()

def convertObjectToModel(arguments):
	inFile = open(arguments["input"], mode="r")
	objlines = inFile.readlines()
	inFile.close()
	
	outFile = open(arguments["output"], mode="w")
	outFile.write('FILETYPE MODEL\n')
	outFile.write('DATA {\n')

	vertices = []
	indices = []	
	texcoords = []
	normals = []
	faceset = []
	meshes = []
	for l in objlines:
		if l[0] == 'v':
			if l[1] == 'n':
				vntokens = l.replace('  ', ' ').split(' ')
#				print(vntokens)
				normals.append([float(vntokens[1]), float(vntokens[2]), float(vntokens[3])])
			elif l[1] == 't':
				vttoken = l.replace('  ', ' ').split(' ')
#				print(vttokens)
				texcoords.append([float(vttoken[1]), float(vttoken[2]), float(vttoken[3])])
			else:
				vtokens = l.replace('  ', ' ').split(' ')
#				print(vtokens)
				vertices.append([float(vtokens[1]), float(vtokens[2]), float(vtokens[3])])
		elif l[0] == 'f':
			ftoken = l.split(' ')
			indexset = []
			for ft in ftoken[1:]:
				fts = ft.replace('  ', ' ').split('/')
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
				indices.append([indexset[pidx][0], indexset[pidx+1][0], indexset[pidx+2][0]])
		elif l[0] == 'o':
			otoken = l.split(' ')
			meshes.append({"Name": otoken[1].replace('\n', ''), "BeginIndex": len(indices)})

	for m in meshes:	
		outFile.write('    MATERIALS {\n')
		outFile.write('        ID %s\n'%m["Name"])
		outFile.write('        SHADER standard\n')
		outFile.write('    }\n')

	print('vertices : %d faceset : %d indices : %d'%(len(vertices), len(faceset), len(indices)))

	outFile.write('    ELEMENTS {\n')
	outFile.write('        MESH {\n')
	outFile.write('            TRANSFORM {\n')
	outFile.write('                POSITION [0.0 0.0 0.0]\n')
	outFile.write('                ROTATION [0.0 0.0 0.0]\n')
	outFile.write('                SCALE [1.0 1.0 1.0]\n')
	outFile.write('            }\n')
	outFile.write('            VERTICES {\n')
	for f in faceset:
		outFile.write('                {\n')
		outFile.write('                    POSITION [%f %f %f]\n'%(vertices[f[0]][0], vertices[f[0]][1], vertices[f[0]][2]))
		outFile.write('                    NORMAL [%f %f %f]\n'%(normals[f[2]][0], normals[f[2]][1], normals[f[2]][2]))
		outFile.write('                    TEXCOORD [%f %f %f]\n'%(texcoords[f[1]][0], texcoords[f[1]][1], texcoords[f[1]][2]))
		outFile.write('                }\n')
	outFile.write('            }\n')
	outFile.write('            INDICES {\n')
	index = 0
	meshindex = 0
	currentMesh = meshes[0]
	for i in indices:
		if meshindex + 1 < len(meshes) and index >= meshes[meshindex+1]["BeginIndex"]:
			meshindex = meshindex + 1
			currentMesh = meshes[meshindex]
		outFile.write('                {\n')
		outFile.write('                    MATERIAL %s\n'%currentMesh["Name"])
		outFile.write('                    POLYGON [%d %d %d]\n'%(i[0], i[1], i[2]))
		outFile.write('                }\n')
		index = index + 1
	outFile.write('            }\n')
	outFile.write('        }\n')
	outFile.write('    }\n')
		
	outFile.write('}\n')
	outFile.close()

def main():
	printTitle()
	
	arguments = getArguments(sys.argv)
	if "input" not in arguments:
		printUsage()
		return 1
	
	print(arguments)
	
	if arguments["command"] == "makesphere":
		makeSphere(arguments)
	elif arguments["command"] == "obj":
		convertObjectToModel(arguments)
	else:
		printUsage()
	
	return 0
	
main()