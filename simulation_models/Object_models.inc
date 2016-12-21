#Object includes
##########################################
#################chair####################
define chair position
(

  size [0.18 0.18 0.20]	
   block #TABLE
   (
      points 4
      point[0]  [ 0   0]
      point[1]  [ 0.4 0]
      point[2]  [ 0.4 0.4]
      point[3]  [ 0   0.4]
      z [0.4 0.45]
   )
   block #LEG1
   (
      points 4
      point[0]  [ 0    0]
      point[1]  [ 0.05 0]
      point[2]  [ 0.05 0.05]
      point[3]  [ 0    0.05]
      z [0 0.4]
   )
   block #LEG2
   (
      points 4
      point[0]  [ 0.35 0]
      point[1]  [ 0.4  0]
      point[2]  [ 0.4  0.05]
      point[3]  [ 0.35 0.05]
      z [0 0.4]
   )
   block #LEG3
   (
      points 4
      point[0]  [ 0.35 0.35]
      point[1]  [ 0.4  0.35]
      point[2]  [ 0.4  0.4]
      point[3]  [ 0.35 0.4]
      z [0 0.4]
   )
   block #LEG4
   (
      points 4
      point[0]  [ 0    0.35]
      point[1]  [ 0.05 0.35]
      point[2]  [ 0.05 0.4]
      point[3]  [ 0    0.4]
      z [0 0.4]
   )

   block #BACK
   (
      points 4
      point[0]  [0   0.35]
      point[1]  [0.4 0.35]
      point[2]  [0.4 0.4]
      point[3]  [0   0.4]
      z [0.45 .8]
   )

   color "brown"	
)
##########################################

##########################################
#################human####################
define human position
(
   fiducial_return 10
   object_return 1
   gripper_return 1
   size [0.05 0.16 0.4]	
   block #LEG
   (
      points 12
      point[0]  [ 1 -15.5]
      point[1]  [ 3 -14.5]
      point[2]  [ 4 -12.5]
      point[3]  [ 4 -10.5]
      point[4]  [ 3 -8.5]
      point[5]  [ 1 -7.5]
      point[6]  [-1 -7.5]
      point[7]  [-3 -8.5]
      point[8]  [-4 -10.5]
      point[9]  [-4 -12.5]
      point[10] [-3 -14.5]
      point[11] [-1 -15.5]
      z [0 0.8]
   )
   block #LEG
   (
      points 12
      point[0]  [ 1  7.5]
      point[1]  [ 3  8.5]
      point[2]  [ 4 10.5]
      point[3]  [ 4 12.5]
      point[4]  [ 3 14.5]
      point[5]  [ 1 15.5]
      point[6]  [-1 15.5]
      point[7]  [-3 14.5]
      point[8]  [-4 12.5]
      point[9]  [-4 10.5]
      point[10] [-3  8.5]
      point[11] [-1  7.5]
      z [0 0.8]
   )
   block #BODY
   (
      points 12
      point[0]  [ 1 -15.5]
      point[1]  [ 3 -14.5]
      point[2]  [ 4 -12.5]
      point[3]  [ 4 12.5]
      point[4]  [ 3 14.5]
      point[5]  [ 1 15.5]
      point[6]  [-1 15.5]
      point[7]  [-3 14.5]
      point[8]  [-4 12.5]
      point[9]  [-4 -12.5]
      point[10] [-3 -14.5]
      point[11] [-1 -15.5]
      z [0.8 1.4]
   )
   block #HEAD
   (
      points 12
      point[0]  [ 1 -6.5]
      point[1]  [ 3 -5.5]
      point[2]  [ 4 -4.5]
      point[3]  [ 4 4.5]
      point[4]  [ 3 5.5]
      point[5]  [ 1 6.5]
      point[6]  [-1 6.5]
      point[7]  [-3 5.5]
      point[8]  [-4 4.5]
      point[9]  [-4 -4.5]
      point[10] [-3 -5.5]
      point[11] [-1 -6.5]
      z [1.4 1.7]
   )

   color "orange"
   #drive "diff"	
)
##########################################

##########################################
#################maniqui##################
define maniqui position
(
  size [0.12 0.47 1.7]	
   block #LEG
   (
      points 12
      point[0]  [ 1 -15.5]
      point[1]  [ 3 -14.5]
      point[2]  [ 4 -12.5]
      point[3]  [ 4 -10.5]
      point[4]  [ 3 -8.5]
      point[5]  [ 1 -7.5]
      point[6]  [-1 -7.5]
      point[7]  [-3 -8.5]
      point[8]  [-4 -10.5]
      point[9]  [-4 -12.5]
      point[10] [-3 -14.5]
      point[11] [-1 -15.5]
      z [0 0.8]
   )
   block #LEG
   (
      points 12
      point[0]  [ 1  7.5]
      point[1]  [ 3  8.5]
      point[2]  [ 4 10.5]
      point[3]  [ 4 12.5]
      point[4]  [ 3 14.5]
      point[5]  [ 1 15.5]
      point[6]  [-1 15.5]
      point[7]  [-3 14.5]
      point[8]  [-4 12.5]
      point[9]  [-4 10.5]
      point[10] [-3  8.5]
      point[11] [-1  7.5]
      z [0 0.8]
   )
   block #BODY
   (
      points 12
      point[0]  [ 1 -15.5]
      point[1]  [ 3 -14.5]
      point[2]  [ 4 -12.5]
      point[3]  [ 4 12.5]
      point[4]  [ 3 14.5]
      point[5]  [ 1 15.5]
      point[6]  [-1 15.5]
      point[7]  [-3 14.5]
      point[8]  [-4 12.5]
      point[9]  [-4 -12.5]
      point[10] [-3 -14.5]
      point[11] [-1 -15.5]
      z [0.8 1.4]
   )
   block #HEAD
   (
      points 12
      point[0]  [ 1 -6.5]
      point[1]  [ 3 -5.5]
      point[2]  [ 4 -4.5]
      point[3]  [ 4 4.5]
      point[4]  [ 3 5.5]
      point[5]  [ 1 6.5]
      point[6]  [-1 6.5]
      point[7]  [-3 5.5]
      point[8]  [-4 4.5]
      point[9]  [-4 -4.5]
      point[10] [-3 -5.5]
      point[11] [-1 -6.5]
      z [1.4 1.7]
   )

   color "gray50"
   #drive "diff"	
)
##########################################

##########################################
#################plantpot#################
define plantpot position
(
  size [0.125 0.125 0.200]	
   block #BASE
   (
      points 8
      point[0]  [ 1  0]
      point[1]  [ 3  0]
      point[2]  [ 4  1]
      point[3]  [ 4  3]
      point[4]  [ 3  4]
      point[5]  [ 1  4]
      point[6]  [ 0  3]
      point[7]  [ 0  1]      
      z [0 0.6]
   )
   block #PLANT
   (
      points 4
      point[0]  [ 1.75 1.75 ]
      point[1]  [ 2.25 1.75 ]
      point[2]  [ 2.25 2.25 ]
      point[3]  [ 1.75 2.25 ]
      z [0.6 1]
   )
   block #LEAF1
   (
      points 7
      point[0]  [ 2.25 1.75 ]
      point[1]  [ 2.25 2.25 ]
      point[2]  [ 2.5  2.5  ]
      point[3]  [ 3    2.5  ]
      point[4]  [ 4    2    ]
      point[5]  [ 3    1.5  ]
      point[6]  [ 2.5  1.5  ] 
      z [1 1.05]
   )
   color "green"	
)
##########################################

##########################################
###############sofa_small#################
define sofa_small position
(
 fiducial_return 4
  size [0.25 0.25 0.250]	
   block #BASE
   (
      points 8
      point[0]  [ 0.5 0]
      point[1]  [ 4.5 0]
      point[2]  [ 5   0.5]
      point[3]  [ 5   3.5]
      point[4]  [ 4.5 4]
      point[5]  [ 0.5 4]
      point[6]  [ 0   3.5]
      point[7]  [ 0   0.5] 
      z [0 0.5]
   )
   block #HAND1
   (
      points 7
      point[0]  [ 0.5 0]
      point[1]  [ 1 0]
      point[2]  [ 1.5   0.5]
      point[3]  [ 1.5   2.5]
      point[4]  [ 0.5   2.5]
      point[5]  [ 0 3]
      point[6]  [ 0   0.5] 
      z [0.5 0.7]
   )
   block #HAND2
   (
      points 7
      point[0]  [ 4 0]
      point[1]  [ 4.5 0]
      point[2]  [ 5   0.5]
      point[3]  [ 5   3]
      point[4]  [ 4.5 2.5]
      point[5]  [ 3.5 2.5]
      point[6]  [ 3.5   0.5] 
      z [0.5 0.7]
   )
   block #BACK
   (
      points 8
      point[0]  [ 0.5 2.5]
      point[1]  [ 4.5 2.5]
      point[2]  [ 5   3]
      point[3]  [ 5   3.5]
      point[4]  [ 4.5 4]
      point[5]  [ 0.5 4]
      point[6]  [ 0   3.5]
      point[7]  [ 0   3] 
      z [0.5 1]
   )
   color "yellow"	
)
##########################################

##########################################
################sofa_large################
define sofa_large position
(
  size [0.5 0.25 0.25]	
   block #BASE
   (
      points 8
      point[0]  [ 0.5 0]
      point[1]  [ 9.5 0]
      point[2]  [ 10   0.5]
      point[3]  [ 10   3.5]
      point[4]  [ 9.5 4]
      point[5]  [ 0.5 4]
      point[6]  [ 0   3.5]
      point[7]  [ 0   0.5] 
      z [0 0.5]
   )
   block #HAND1
   (
      points 7
      point[0]  [ 0.5 0]
      point[1]  [ 1 0]
      point[2]  [ 1.5   0.5]
      point[3]  [ 1.5   2.5]
      point[4]  [ 0.5   2.5]
      point[5]  [ 0 3]
      point[6]  [ 0   0.5] 
      z [0.5 0.7]
   )
   block #HAND2
   (
      points 7
      point[0]  [ 9 0]
      point[1]  [ 9.5 0]
      point[2]  [ 10   0.5]
      point[3]  [ 10   3]
      point[4]  [ 9.5 2.5]
      point[5]  [ 8.5 2.5]
      point[6]  [ 8.5   0.5] 
      z [0.5 0.7]
   )
   block #BACK
   (
      points 8
      point[0]  [ 0.5 2.5]
      point[1]  [ 9.5 2.5]
      point[2]  [ 10   3]
      point[3]  [ 10   3.5]
      point[4]  [ 9.5 4]
      point[5]  [ 0.5 4]
      point[6]  [ 0   3.5]
      point[7]  [ 0   3] 
      z [0.5 1]
   )
   color "yellow"	
)
##########################################
#################table####################
define table position
(
  size [0.5 0.25 0.125]	
   block #TABLE
   (
      points 4
      point[0]  [ 0  0]
      point[1]  [ 1.5 0]
      point[2]  [ 1.5 0.75]
      point[3]  [ 0 0.75]
      z [0.75 0.8]
   )
   block #LEG1
   (
      points 4
      point[0]  [ 0    0]
      point[1]  [ 0.05 0]
      point[2]  [ 0.05 0.05]
      point[3]  [ 0    0.05]
      z [0 0.75]
   )
   block #LEG2
   (
      points 4
      point[0]  [ 1.45 0]
      point[1]  [ 1.5  0]
      point[2]  [ 1.5  0.05]
      point[3]  [ 1.45 0.05]
      z [0 0.75]
   )
   block #LEG3
   (
      points 4
      point[0]  [ 1.45 0.7]
      point[1]  [ 1.5  0.7]
      point[2]  [ 1.5  0.75]
      point[3]  [ 1.45 0.75]
      z [0 0.75]
   )
   block #LEG4
   (
      points 4
      point[0]  [ 0    0.7]
      point[1]  [ 0.05 0.7]
      point[2]  [ 0.05 0.75]
      point[3]  [ 0    0.75]
      z [0 0.75]
   )
   color "brown"	
)
##########################################

##########################################
#############table_circular###############
define table_circular position
(
  size [0.25 0.25 0.15]	
   block #table
   (
      points 12
      point[0]  [ 1 -15.5]
      point[1]  [ 3 -14.5]
      point[2]  [ 4 -12.5]
      point[3]  [ 4 -10.5]
      point[4]  [ 3 -8.5]
      point[5]  [ 1 -7.5]
      point[6]  [-1 -7.5]
      point[7]  [-3 -8.5]
      point[8]  [-4 -10.5]
      point[9]  [-4 -12.5]
      point[10] [-3 -14.5]
      point[11] [-1 -15.5]
      z [0 0.8]
   )
   color "blue"	
)
##########################################

##########################################
###############trash_bin##################
define trash_bin position
(
  size [0.125 0.125 0.125]	
   block #trash bin
   (
      points 12
      point[0]  [ 1 -15.5]
      point[1]  [ 3 -14.5]
      point[2]  [ 4 -12.5]
      point[3]  [ 4 -10.5]
      point[4]  [ 3 -8.5]
      point[5]  [ 1 -7.5]
      point[6]  [-1 -7.5]
      point[7]  [-3 -8.5]
      point[8]  [-4 -10.5]
      point[9]  [-4 -12.5]
      point[10] [-3 -14.5]
      point[11] [-1 -15.5]
      z [0 0.4]
   )
   color "red"	
)
##########################################
