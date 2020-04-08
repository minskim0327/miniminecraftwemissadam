# mini-minecraft-wemissadam
mini-minecraft-wemissadam created by GitHub Classroom

Chang Hun Lee - Procedural Terrain 
Elaine Moon - Efficient Terrain Rendering and Chunking
Min Suk Kim - Game Engine Tick Function and Player Physics

Chang Hun Lee (Procedural Terrain)
- For grassland heights, I used Worley noise to the x and z positions divided by 64. To get more uneven noise, I applied perlin noise to the input UV and multiplied its absolute value with the Worley noise. As for mountain heights, to get more jagged effects, I applied Perline noise to the x and z positions divided by 32. For sharper contrast, I took the absolute value of the noise and applied smoothstep(0.25, 0.75, noise). For even sharper contrast, I took the result to its second power. For the third interpolating function, I used Perlin noise to get t and applied smoothstep(0.4, 0.6, t) after remapping t to (0, 1). Any value of t below 0.5 was set to grassland and above 0.5 was set to mountain height.  

Elaine Moon (Efficient Terrain Rendering and Chunking) 
- I implemented this features by first modifying terrain and chunk classes so instead of rendering every single cube mesh 
  in a chunk, I render each chunk mesh. Then, I modified the vbo data format to make sure it sends a per-vertex data in a 
  interleaved format by modifying the chunk, drawable, and shaderprogram classes. Finally, I added a feature to newly create
  forward, backward, right, left chunks based on the player's position in the scene. Although the chunks are added 
  to the terrain class and never deleted, it might deload visually in the scene if it is not within the frame of 
  minX, maxX, minZ, maxZ of the player's current position in renderTerrain().
  
