# mini-minecraft-wemissadam
mini-minecraft-wemissadam created by GitHub Classroom

<MS1>
Chang Hun Lee - Procedural Terrain 
Elaine Moon - Efficient Terrain Rendering and Chunking
Min Suk Kim - Game Engine Tick Function and Player Physics

Chang Hun Lee (Procedural Terrain)
- For grassland heights, I used Worley noise to the x and z positions divided by 64. To get more uneven noise, I applied perlin
noise to the input UV and multiplied its absolute value with the Worley noise. As for mountain heights, to get more jagged effects,
 I applied Perline noise to the x and z positions divided by 32. For sharper contrast, I took the absolute value of the noise and 
applied smoothstep(0.25, 0.75, noise). For even sharper contrast, I took the result to its second power. For the third interpolating
function, I used Perlin noise to get t and applied smoothstep(0.4, 0.6, t) after remapping t to (0, 1). Any value of t below 0.5 was
set to grassland and above 0.5 was set to mountain height.  

Elaine Moon (Efficient Terrain Rendering and Chunking) 
- I implemented this features by first modifying terrain and chunk classes so instead of rendering every single cube mesh 
  in a chunk, I render each chunk mesh. Then, I modified the vbo data format to make sure it sends a per-vertex data in a 
  interleaved format by modifying the chunk, drawable, and shaderprogram classes. Finally, I added a feature to newly create
  forward, backward, right, left chunks based on the player's position in the scene. Although the chunks are added 
  to the terrain class and never deleted, it might deload visually in the scene if it is not within the frame of 
  minX, maxX, minZ, maxZ of the player's current position in renderTerrain().
  
  
Min Seok Kim (Game Engine Tick Function and Player Physics)
- I implemented the smootehr movement of Game by utilizing QDateTime::currentMSecsSinceEpoch() so that every displacement is updated (at most and around) 60 fps, instead of harsh movement that only corresponds when there were keyboard inputs. I utilized the pre-defined strcut called InputBundles, which keep track of the player's keyboard and physical state. I also utilize this input bundle to address the displacement of the player, divided into three functions : Player::tick()( called from MyGL::tick()) which calls processInputs() and computePhysics). I differentiated the wasd movement bewteen flightmode and nonflightmode. In flight mode, the player is able to spectate, analogous to Overwatch spectater mode (except that in Overwatch, spectator detects partial collision). In non-flight mode, the player is able to jump(currenlty having minor issues), and also move in x, z coordinates by wasd keys. However, in non-flight mode, I disregarded the y componenet and re-normalized the velocity vector so that the player stays on the ground while walking. To make the game engine simulate more closer to reality, I also mimicked some terminal velocity effect by scaling down the velocity to its 90 % whenever the computePhysics() function is called. Lastly, I implemented the collision effect by giving a small epsilon value to the block so that the player could "stick" to the block. This algorithm, though, is to be modifed later on by instead of sending one ray per one vertex of the player mesh, it sends three rays from each vertex that are parrell to the y axis, and in each axis move the maximum amount the player can.
Besides player's phycis, I also handled different actions regarding mouse events. I enabled the user to see the actual aim (just like minecraft) by changing calling setCursor(Qt::CrossCursor) in MyGL.cpp. Also, I handled the mouse to rotate globally on x and y axis to implement behaviors of polar coordinates. After calculation of displacement of mouse was finished, I then set the mouse position to be at centered (so that the "aim" could be fixed) by calling moveMouseToCenter() function in MyGL. However, there arose a problem: the code works perfectly fine on windows, but the code doesn't work in Mac. Another type of mouse event that I handled were left and right clicks. When the user left clicks, it detects any block's existence within 3 units and turn that into "EMPTY" block. When the user right clicks, it creates a new "STONE" block adjacent to the block face they are looking at (within 3 units' distance). Both codes, however, worked fine until merging, but after merging it only detects collision. 


<MS2> 

Chang Hun Lee - L-System Rivers
Elaine Moon - Texturing and Texture Animation
Min Suk Kim - Multithreaded Terrain Generation

Elaine Moon (Texturing and Texture Animation)
- I implemented the texturing features by first setting up texture class so that we can load an image as textures. I reorganized chunk.create() function so that it recognizes each blocktype and saves the uv coordinates instead of color. Then, I made sure that there are separate vbo data for blocks that are transparent and opaque. (Separate pos, uvs, nor, idx) I also had to put a flag variable in the vbo data for per-vertex because I needed the GPU side to be alarmed when the blocktypes are water and lava. This added another float to the interleaved vbo data. Implementing this part included changes in shaderprogram class and drawable class in order to match the GPU side handlers correctly. After I made changes to lambert.frag.glsl and lambert.vert.glsl in order for them to support functions like reading a texture and time. I get the time variable from mygl class where I increment time everytime paintGL() is called. In lambert.frag.glsl, I used the time variable to make the water and lava blocks move their uv coordinates by some offset to give an illusion that they are moving. 

Chang Hun Lee (L-System Rivers)
- I implemented lsystem rivers by making a Turtle class and River class. The Turtle class acts as explained in the lecture and the River class stores stack of Turtles. For randomness, I used random number to decide if the river is split in two ways or three ways. I also made the angle rotation of the river between 30 and 60 degrees randomly. I used the rule 'F=FF' to keep track of depth, which was then used to determine the depth and width of the river branches. I used two iterations to expand the grammar so that the maximum depth would be 3. To actually draw the branches, in the GoForward() function, I used the sine function to get steps of curves and rotated them according the the orientation of the branch. I additionally curved the terrain around the river branch by about 3 blocks to make the curving look smooth.  
