import json

with open("data.txt") as f:
    content = f.readlines()

content = [x.strip() for x in content]
l=[]
for lines in content:
    if not lines.startswith('#'):
        l.append(lines)

s = ''.join(l)
s = s.replace(' ','')
s = s.replace('---','')
s = s.replace('-','')
s = s.replace('>','')
s = s.replace('[','')
s = s.replace(']','')
s = s.replace('{','')
s = s.replace('}','')
s = s.replace('<','')
s = s.replace(':','')
s = s.replace('Accept','True')
s = s.replace('Discard','False')



data = ""
for lines in s:
    data = data + lines


data =  data.split(',')


dict = {}

for lines in data:
    words = lines.split('=')
    dict[words[0]] = words[1]





#print(dict)

with open('data.json', 'w') as fp:
    json.dump(dict, fp)
