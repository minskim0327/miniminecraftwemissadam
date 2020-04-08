# mini-minecraft-wemissadam
mini-minecraft-wemissadam created by GitHub Classroom

Chang Hun Lee - Procedural Terrain 
Elaine Moon - Efficient Terrain Rendering and Chunking
Min Suk Kim - Game Engine Tick Function and Player Physics

Elaine Moon (Efficient Terrain Rendering and Chunking) 
- I implemented this features by first modifying terrain and chunk classes so instead of rendering every single cube mesh 
  in a chunk, I render each chunk mesh. Then, I modified the vbo data format to make sure it sends a per-vertex data in a 
  interleaved format by modifying the chunk, drawable, and shaderprogram classes. Finally, I added a feature to newly create
  forward, backward, right, left chunks based on the player's position in the scene. Although the chunks are added 
  to the terrain class and never deleted, it might deload visually in the scene if it is not within the frame of 
  minX, maxX, minZ, maxZ of the player's current position in renderTerrain().
  
