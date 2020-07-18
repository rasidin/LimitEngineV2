import sys
import numpy as np
import OpenEXR, Imath, array

def printTitle():
	print('EXR converter')
	print('exr resizing')
	print('author minseob')

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
	printTitle()
	
	argv = sys.argv
	argc = len(sys.argv)
	args = getArguments(argc, argv)
	print(args)
	
	if 'cmd' not in args:
		return
	command = args['cmd']
	
	inputpath = args['in']
	outputpath = args['out']
	
	imgWidth = int(args['width'])
	imgHeight = int(args['height'])
	imgXOffset = 0
	imgYOffset = 0
	
	pixeltype = Imath.PixelType(Imath.PixelType.FLOAT)
	inputEXR = OpenEXR.InputFile(inputpath)
	r_str, g_str, b_str = inputEXR.channels('RGB', pixeltype)
	r_float = np.array(array.array('f', r_str))
	g_float = np.array(array.array('f', g_str))
	b_float = np.array(array.array('f', b_str))
	dw = inputEXR.header()['dataWindow']
	inputSize = (int(dw.max.x - dw.min.x) + 1, int(dw.max.y - dw.min.y) + 1)

	imgZoomRate = imgWidth / inputSize[0]
	imgConvHeight = imgZoomRate * inputSize[1]
	if imgConvHeight >= imgHeight :
		imgYOffset = int((imgConvHeight - imgHeight) / 2)
	else:
		imgZoomRate = imgHeight / inputSize[1]
		imgConvWidth = imgZoomRate * inputSize[1]
		imgXOffset = int((imgConvWidth - imgWidth) / 2)
	
	r_out = np.resize([], imgWidth * imgHeight)
	g_out = np.resize([], imgWidth * imgHeight)
	b_out = np.resize([], imgWidth * imgHeight)
	outputEXR = OpenEXR.OutputFile(outputpath, OpenEXR.Header(imgWidth, imgHeight))

	print('(%d, %d) -> (%d, %d) + (%d, %d)'%(inputSize[0], inputSize[1], imgWidth, imgHeight, imgXOffset, imgYOffset))
	for y in range(imgHeight):
		for x in range(imgWidth):
			read_x = int(x / imgZoomRate) + imgXOffset
			read_y = int(y / imgZoomRate) + imgYOffset
			readindex = read_x + read_y * inputSize[0]
			writeindex = x + y * imgWidth
			r_out[writeindex] = r_float[readindex]
			g_out[writeindex] = g_float[readindex]
			b_out[writeindex] = b_float[readindex]
			
	r_out_str = array.array('f', r_out).tostring()
	g_out_str = array.array('f', g_out).tostring()
	b_out_str = array.array('f', b_out).tostring()
	outputEXR.writePixels({'R': r_out_str, 'G': g_out_str, 'B': b_out_str})
	outputEXR.close()
	
if __name__ == '__main__':
	main()