blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
yellow = gr.material({1.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
grey = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 10)

rootNode = gr.node( 'rootSkybox' )

jSkybox = gr.joint( 'jSkybox', {0, 0, 0}, {0, 0, 0})
skybox = gr.mesh( 'uvcube', 'Skybox' )
skybox:set_material( blue )
skybox:scale(5000, 5000, 5000)
jSkybox:add_child(skybox)
jSkybox:translate(0, 4000, 0)

jField = gr.joint( 'jField', {-90,0,90}, {0,0,0} )
field = gr.mesh( 'plane', 'Field' )
field:set_material( green )
field:scale(50, 50, 1)
--field:scale(5000, 5000, 1)
jField:rotate('x', -90);
--jField:translate(0, -100.0, 0)
jField:translate(0, -1.0, 0)
jField:add_child(field)

jWall1 = gr.joint( 'jWall1', {0, 0, 0}, {0, 0, 0} )
gWall1 = gr.mesh( 'plane', 'gWall1' )
gWall1:set_material(grey)
gWall1:scale( 5, 5, 1 )
jWall1:translate( 0, 4.5, -10 )
jWall1:add_child( gWall1 )

rootNode:add_child( jSkybox )
rootNode:add_child( jWall1 )
rootNode:add_child( jField )

return rootNode
