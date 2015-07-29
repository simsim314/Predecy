import golly as g 

rect = g.getselrect()

def GetIndex(x, y):
	idx = 1
	result = 0
	for j in xrange(y, y + 3):
		for i in xrange(x, x + 3):
			if g.getcell(i, j) == 1:
				result += idx
			idx *= 2
	return result
	
result = ""

for y in xrange(rect[1], rect[1] + rect[3], 3):
	for x in xrange(rect[0], rect[0] + rect[2], 3):
		result += "srch.SetupBoxValue({1}, {2}, {0});\n".format(GetIndex(x, y), (x - rect[0]) / 3 + 1 , (y - rect[1]) / 3 + 1)
		
g.setclipstr(result)