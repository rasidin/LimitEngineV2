import sys
from PIL import Image, ImageDraw, ImageFont

def printTitle():
	print('TTF2LEFont V.0.1')
	print('by minseob')

def printUsage():
	print('Usage : ttf2lefont [fontpath] [outputfilename]')
	print('Options : ')
	print('[b] BeginCode : Begin Ascii code')
	print('[e] EndCode : End Ascii code')
	print('[s] SizeOfFont')
	print('[w] Width : Output image width')
	print('[h] Height : Output image height')

def getArguments(argc, argv):
	output = {}
	if (argc < 3):
		return []
		
	for argvalue in argv[1:]:
		if argvalue[0] == '-':
			subargs = argvalue[1:].split('=')
			output[subargs[0]]= int(subargs[1])
			continue
		else:
			if 'fontpath' in output:
				output['output'] = argvalue
			else:
				output['fontpath'] = argvalue
	
	return output

def writeLEFontXML(args, sizes):
	out_xml = open(args['output'] + '.xml', 'w')
	out_xml.write('<?xml version="1.0"?>\n')
	out_xml.write('<glyphs>\n')
	for idx in range(len(sizes)):
		out_xml.write('    <chr>\n')
		ascii = args['b'] + idx
		out_xml.write('        <ascii>%d</ascii>\n'%ascii)
		text = chr(ascii)
		out_xml.write('        <text>%s</text>\n'%text)
		out_xml.write('        <x>%d</x>\n'%sizes[idx][0])
		out_xml.write('        <y>%d</y>\n'%sizes[idx][1])
		out_xml.write('        <w>%d</w>\n'%sizes[idx][2])
		out_xml.write('        <h>%d</h>\n'%sizes[idx][3])
		out_xml.write('    </chr>\n')
	out_xml.write('</glyphs>')
	out_xml.close()

def writeLEFontTable(args, sizes):
    out_text = open(args['output'] + '.text', 'w')
    out_text.write('FILETYPE FONT\n')
    out_text.write('DATA {\n')
    out_text.write('    GLYPHS {\n')
    for idx in range(len(sizes)):
        ascii = args['b'] + idx
        text = chr(ascii)
        out_text.write('        GLYPH {\n')
        out_text.write('            ASCII %d\n'%ascii)
        if text == "\"" or text == "\\":
            out_text.write('            TEXT \"\\' + text + '\"\n')
        else:
            out_text.write('            TEXT \"' + text + '\"\n')
        out_text.write('            RECT [ %d %d %d %d ]\n'%(sizes[idx][0], sizes[idx][1], sizes[idx][2], sizes[idx][3]))
        out_text.write('        }\n')
    out_text.write('    }\n')
    out_text.write('}\n')
    out_text.close()

def writeLEFont(args):
	out_png = Image.new('RGBA', (args['w'], args['h']), (0,0,0,0))
	out_draw = ImageDraw.Draw(out_png)
	out_size = []
	font = ImageFont.truetype(args['fontpath'], args['s'])
	
	currentX = 0
	currentY = 0
	maxHeight = 0
	for idx in range(args['e'] - args['b'] + 1):
		text = '' + chr(args['b'] + idx)
		text_size = out_draw.textsize(text, font=font)
		
#		print(text + ' : %d'%(currentX) + ' : %d x %d'%(text_size[0], text_size[1]))
		
		if (currentX + text_size[0]) > args['w']:
			currentY = currentY + maxHeight
			currentX = 0
			maxHeight = 0
		
		out_size.append([currentX, currentY, text_size[0], text_size[1]])
		
		out_draw.text((currentX, currentY), text, font=font, fill=(255,255,255))
		currentX = currentX + text_size[0]
		maxHeight = max(maxHeight, text_size[1])
		
#	writeLEFontXML(args, out_size)
	writeLEFontTable(args, out_size)
    
	out_png.save(args['output'] + '.tga')

def main():
	printTitle()
	
	arguments = getArguments(len(sys.argv), sys.argv)
	if ('output' not in arguments):
		printUsage()
		return
		
	if ('b' not in arguments):
		arguments['b'] = 32
	if ('e' not in arguments):
		arguments['e'] = 126
	if ('s' not in arguments):
		arguments['s'] = 9
	if ('w' not in arguments):
		arguments['w'] = 256
	if ('h' not in arguments):
		arguments['h'] = 256		
		
	print arguments
	
	writeLEFont(arguments)

if __name__ == '__main__':
	main()
