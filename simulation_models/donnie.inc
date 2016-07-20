########################################################
############# Definition of Donnie #####################



define donnie_sonar sensor
(
    # define the size of each transducer [xsize ysize zsize] in meters
    size [0.01 0.05 0.01 ] 
    # define the range bounds [min max]
    range [0.3 5.0]
    # define the angular field of view (by default in degrees)
    fov 15
    # define the color that ranges are drawn in the gui
    color_rgba [ 0.5 0.8 1 0.5]
)

define donnie_sonars ranger
( 
  # one line for each sonar [xpos ypos zpos heading]
  donnie_sonar( pose [ 0.51 0 -0.5 0])
  donnie_sonar( pose [ -0.51 0 -0.5 -180])
  donnie_sonar( pose [ 0.30 0.30 -0.5 45])
  donnie_sonar( pose [ 0.30 -0.30 -0.5 -45])
  donnie_sonar( pose [ -0.30 0.30 -0.5 -225])
  donnie_sonar( pose [ -0.30 -0.30 -0.5 225])

)

define donnie_bumper bumper
( 
	size [0.1 0.1 0.01 0]
	pose [0 0 -0.11 0]
	bcount 4  
	blength 0.098
	bpose[0] [0.057  0.045 .02  45]
	bpose[1] [0.057 -0.045 .02 -45] 
	bpose[2] [-0.057  0.045 .02 -45]
	bpose[3] [-0.057 -0.045 .02 45]
)



define donnie_eyes blobfinder
(
	colors_count 6
	colors ["orange" "DarkBlue" "brown" "green" "yellow" "red"]
	image [160 120] # resolution
	range 5.00 		 # meters
	fov 60 # fov (in unit_angle (degrees by def))
	size [ 0 0 0 ]
	pose [0.65 0 -1 0]
)

define donnie_HS ranger
(
	donnie_sonar( pose [ 0.51 0 -1 0])
)

define donnie_head position
(
	drive "diff"

	# actual size
	size [0.06397 0.06397 0.069]
	# centre of rotation offset
	origin [0.000 0.000 0.000 0.000]

	block
	(
		points 8
		point[7] [0 0.03747]
		point[6] [0 0.09047]
		point[5] [0.03747 0.12794]
		point[4] [0.09047 0.12794]
		point[3] [0.12794 0.09047]
		point[2] [0.12794 0.03747]
		point[1] [0.09047 0]
		point[0] [0.03747 0]
		z [0 1]
	)

	obstacle_return 0
	bumper_return 0	

	donnie_HS()
	donnie_eyes( )
	
)

define donnie position
(
	# actual size
	size [0.12794 0.12794 0.120]
	# centre of rotation offset
	origin [0.000 0.000 0.000 0.000]
	blob_return 0

	# the shape of donnie
	block
	(
		points 8
		point[7] [0 0.03747]
		point[6] [0 0.09047]
		point[5] [0.03747 0.12794]
		point[4] [0.09047 0.12794]
		point[3] [0.12794 0.09047]
		point[2] [0.12794 0.03747]
		point[1] [0.09047 0]
		point[0] [0.03747 0]
		z [0 1]
	)
	obstacle_return 0
	#positional things
	drive "diff"


	#sensors attached to donnie
	donnie_sonars( )
	donnie_bumper( )
	donnie_head( )
)

