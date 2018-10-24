import yaml

stream = file('house_donnie.yaml', 'r')
data = yaml.load(stream)
#print yaml.dump(data)

print "nome do cenario:", data['name']
print "descricao: ", data['description']
print "nro de comodos:", len(data['rooms'])

#pos = [-1, 2] # at donnie room 
pos = [1.47, 0] # at donnie's parents room 

print "Posicao atual ", pos 

for room in data['rooms']:
	#print room['bl_pos'], room['tr_pos']
	#print room['bl_pos'][1], room['tr_pos'][0]
	if pos[0] < room['tr_pos'][0] and pos[0] > room['bl_pos'][0] and pos[1] < room['tr_pos'][1] and pos[1] > room['bl_pos'][1] :
		print "estou na sala ", room['name'], " que possui ", room['area']
		print "the doors are at: "
		for door in room['entrace']:
			print door
		
