import sys
import os
import csv
import array

import numpy
import rawpy
import OpenEXR

def printLog():
	print('NEF to EXR')
	print('NEF raw images to linear color space exr')
	print('author minseob')

def printUsage():
	print('Usage : nef2exr <filelist> <outputfilename>')

def getInputFileInfo(csvfilepath):
	output = []
	if (os.path.exists(csvfilepath)):
		with open(csvfilepath) as csvfile:
			csvline = csv.reader(csvfile)
			for row in csvline:
				output.append(row)
	return output

def getArguments(argc, argv):
	output = {}
	for argidx in range(1, argc):
		if argv[argidx][0] == '-':
			output[argv[argidx].split('=')[0][1:]] = argv[argidx].split('=')[1]
		elif 'in' in output:
			output['out'] = argv[argidx]
		else:
			output['in'] = argv[argidx]
	return output;

def main():
	printLog()
	
	argv = sys.argv
	argc = len(argv)

	if (argc < 3):
		printUsage()
		return -1
	
	args = getArguments(argc, argv)
	print(args)
	
	csvfilepath = args['in']
	outputfilepath = args['out']
	
	filelist = getInputFileInfo(csvfilepath)
	if (len(filelist) == 0):
		return -1

	maxev = 0
	for filedata in filelist:
		if maxev < float(filedata[1]):
			maxev = float(filedata[1])
	maxnits = pow(2, maxev-3)
	print('MaxEV = %d'%maxev)

	pxstep = 1
	if 'step' in args:
		pxstep = int(args['step'])
	imgwidth = 0
	imgheight = 0
	rootdir = os.path.dirname(csvfilepath)
	rawdatalist = []
	mergedRed = []
	mergedGreen = []
	mergedBlue = []
	for filedata in filelist:
		filefullpath = filedata[0]
		if (os.path.isabs(filedata[0]) is False):
			filefullpath = os.path.join(rootdir, filedata[0])
	
		ev = float(filedata[1])
		exposure = pow(2, ev-3)
		if os.path.exists(filefullpath):
			print('Open file : ' + filefullpath)
			print('EV : %f -> %f'%(ev, exposure))
		
			print('Loading...')
			rawdata = rawpy.imread(filefullpath)
			print('Converting...')
			raw2rgb = rawdata.postprocess(gamma=(1,1), no_auto_bright=True, output_bps=16)
			imgwidth =  int(len(raw2rgb[0]) / pxstep)
			imgheight = int(len(raw2rgb)    / pxstep)

			print('Merging %dx%d -> %dx%d...'%(rawdata.sizes.raw_width, rawdata.sizes.raw_height, imgwidth, imgheight))
			
			if len(mergedRed) == 0:
				mergedRed = numpy.resize([], int(imgwidth * imgheight))
			if len(mergedGreen) == 0:
				mergedGreen = numpy.resize([], int(imgwidth * imgheight))
			if len(mergedBlue) == 0:
				mergedBlue = numpy.resize([], int(imgwidth * imgheight))
			
			progress = 0
			sys.stdout.write('[')
			y = 0
			for y in range(imgheight):
				convY = y * pxstep
				curprog = y * 30 / imgheight
				if progress != curprog:
					progress = curprog
					sys.stdout.write('-')
				x = 0
#				print 'y=%d'%y
				for x in range(imgwidth):
					convX = x * pxstep
					index = x + y * imgwidth
#					print 'convx=%d convy=%d'%(convX, convY)
					r = raw2rgb[convY][convX][0]
					g = raw2rgb[convY][convX][1]
					b = raw2rgb[convY][convX][2]
					fr = float(r) / 32767.0 * exposure
					fg = float(g) / 32767.0 * exposure
					fb = float(b) / 32767.0 * exposure
					if r > 1 and r < 32760:
						mergedRed[index] = fr
					elif fr >= maxnits:
						mergedRed[index] = fr						
					if g > 1 and g < 32760:
						mergedGreen[index] = fg
					elif fg >= maxnits:
						mergedGreen[index] = fg
					if b > 1 and b < 32760:
						mergedBlue[index] = fb
					elif fb >= maxnits:
						mergedBlue[index] = fb
#				print 'next'
			print(']')
			
	outputEXR = OpenEXR.OutputFile(outputfilepath, OpenEXR.Header(imgwidth, imgheight))
	
	dataRed = array.array('f', mergedRed).tostring()
	dataGreen = array.array('f', mergedGreen).tostring()
	dataBlue = array.array('f', mergedBlue).tostring()
	outputEXR.writePixels({'R': dataRed, 'G': dataGreen, 'B': dataBlue})
	
	outputEXR.close()

if __name__ == '__main__':
	main()
