-- puppet.lua

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
grey = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
yellow = gr.material({1.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
orange = gr.material({1.0, 165/255, 0.0}, {0.1, 0.1, 0.1}, 10)

function Torso(name)
    local torso = gr.mesh( 'uvsphere', name )
    torso:scale( 1.0, 1.5, 1.0 )
    torso:set_material( white )
    return torso
end

function Shoulders(name)
    local shoulders = gr.mesh( 'uvsphere', name )
    shoulders:scale( 1.4, 0.6, 0.7 )
    shoulders:set_material( black )
    return shoulders;
end

function Neck(name)
    local neck = gr.mesh( 'uvsphere', name )
    neck:scale( 0.3, 0.6, 0.3 )
    neck:set_material( black )
    return neck
end

function Hips(name)
    local hips = gr.mesh( 'uvsphere', name )
    hips:scale( 1.0, 0.4, 0.9 )
    hips:set_material( orange )
    return hips
end

function Thigh(name)
    local thigh = gr.mesh('uvsphere', name )
    thigh:scale( 0.4, 0.8, 0.4 )
    thigh:set_material( orange )
    return thigh
end

function Calf(name)
    local calf = gr.mesh('uvsphere', name )
    calf:scale(0.25, 0.7, 0.25)
    calf:set_material( white )
    return calf 
end

function Foot(name)
    local foot = gr.mesh( 'uvsphere', name )
    foot:scale( 0.2, 0.15, 0.35 )
    foot:set_material( red )
    return foot
end

function UpperArm(name)
    local upperArm = gr.mesh( 'uvsphere', name )
    upperArm:scale( 0.25, 0.75, 0.25 )
    upperArm:set_material( black )
    return upperArm
end

function Forearm(name)
    local forearm = gr.mesh( 'uvsphere', name )
    forearm:scale( 0.2, 0.6, 0.2 )
    forearm:set_material( black )
    return forearm 
end

function Finger(name)
    local finger = gr.mesh( 'uvsphere', name )
    finger:scale( 0.2, 0.2, 0.2 )
    finger:set_material( white )
    return finger 
end

function Hand(name)
    local hand = gr.mesh( 'uvsphere', name )
    hand:scale( 0.15, 0.15, 0.1 )
    hand:set_material( black )
    finger1 = Finger( 'finger1' )
    finger1:translate( -0.6, -0.8, 0.0 )
    hand:add_child( finger1 )

    finger2 = Finger( 'finger2' )
    finger2:translate( 0.0, -1.0, 0.0 )
    hand:add_child( finger2 )

    finger3 = Finger( 'finger3' )
    finger3:translate( 0.6, -0.8, 0.0 )
    hand:add_child( finger3 )

    return hand
end

function Eye(name)
    local eye = gr.mesh( 'uvsphere', name )
    eye:scale( 0.3, 0.3, 0.3 )
    eye:set_material( black )

    local eye_ball = gr.mesh('uvsphere', 'ball'..name)
    eye_ball:scale(0.5, 0.5, 0.5)
    eye_ball:translate(0.0, 0.0, 0.65)
    eye_ball:set_material(white)
    eye:add_child(eye_ball)

    local iris = gr.mesh('uvsphere', 'iris'..name)
    iris:scale(0.6, 0.6, 0.6)
    iris:translate(0.0, 0.0, 0.65)
    iris:set_material(green)
    eye_ball:add_child(iris)

    local pupil = gr.mesh('uvsphere', 'pupil'..name)
    pupil:scale(0.5, 0.5, 0.5)
    pupil:translate(0.0, 0.0, 0.8)
    pupil:set_material(black)
    iris:add_child(pupil)

    return eye
end

function Ear(name)
    local ear = gr.mesh( 'uvsphere', name )
    ear:scale( 0.3, 0.3, 0.1 )
    ear:set_material( black )
    return ear
end

function Stash(name)
    local stash = gr.mesh( 'uvsphere', name )
    stash:scale( 0.015, 0.015, 0.015 )
    stash:set_material( black )
    return stash
end

function Head(name)
    head = gr.mesh('uvsphere', name )
    head:scale( 1.0, 1.0, 1.0 )
    head:translate(0.0, 1.1, 0.0)
    head:set_material(white)
    
    right_eye = Eye('right')
    right_eye:translate(-0.4, 0.2, 0.7)
    head:add_child(right_eye)
    
    left_eye = Eye('left')
    left_eye:translate(0.4, 0.2, 0.7)
    head:add_child(left_eye)
    
    right_ear = Ear('right')
    right_ear:translate(-0.6, 0.7, 0.4)
    head:add_child(right_ear)
    
    left_ear = Ear('left')
    left_ear:translate(0.6, 0.7, 0.4)
    head:add_child(left_ear)
    
    mouth = gr.mesh('uvsphere', 'mouth')
    mouth:scale(0.4, 0.4, 0.4)
    mouth:translate(0.0, -0.3, 0.8);
    mouth:set_material(white)
    head:add_child(mouth)
    
    nose = gr.mesh('uvsphere', 'nose')
    nose:scale(0.25, 0.1, 0.1)
    nose:translate(0.0, -0.2, 1.1)
    nose:set_material(black)
    head:add_child(nose)
    
    stash_one_left = Stash('one_left')
    stash_one_left:translate(-0.1, -0.35, 1.19)
    head:add_child(stash_one_left)
    
    stash_two_left = Stash('two_left')
    stash_two_left:translate(-0.17, -0.33, 1.17)
    head:add_child(stash_two_left)
    
    stash_three_left = Stash('three_left')
    stash_three_left:translate(-0.24, -0.31, 1.15)
    head:add_child(stash_three_left)
    
    stash_one_right = Stash('one_right')
    stash_one_right:translate(0.1, -0.35, 1.19)
    head:add_child(stash_one_right)
    
    stash_two_right = Stash('two_right')
    stash_two_right:translate(0.17, -0.33, 1.17)
    head:add_child(stash_two_right)
    
    stash_three_right = Stash('three_right')
    stash_three_right:translate(0.24, -0.31, 1.15)
    head:add_child(stash_three_right)
    
    lips = gr.mesh('uvsphere', 'lips')
    lips:scale(0.2, 0.01, 0.01)
    lips:set_material(black)
    lips:translate(0.0, -0.53, 1.13)
    head:add_child(lips)

    return head
end

function Create_goalie()
    jPuppet = gr.joint( 'jPuppet', {0, 0, 0}, {0, 0, 0} )

    jPuppet:scale( 0.25, 0.25, 0.25 )
    
    -- Create joints with no rotation
    jTorso      = gr.joint( 'jTorso', {0, 0, 0}, {0, 0, 0} )
    jShoulders  = gr.joint( 'jShoulders', {0, 0, 0}, {0, 0, 0} )
    jHips       = gr.joint( 'jHips', {0, 0, 0}, {0, 0, 0} )
    
    -- Create joints with rotation
    jBaseNeck       = gr.joint( 'jBaseNeck', {-45, 0, 45}, {0, 0, 0} )
    jTopNeck        = gr.joint( 'jTopNeck', {-45, 0, 45}, {-90, 0, 90} )
    jLeftShoulder   = gr.joint( 'jLeftShoulder', {-180, 0, 90}, {0, 0, 0} )
    jLeftElbow      = gr.joint( 'jLeftElbow', {-90, 0, 0}, {0, 0, 0} )
    jLeftWrist      = gr.joint( 'jLeftWrist', {-45, 0, 45}, {0, 0, 0} )
    jRightShoulder  = gr.joint( 'jRightShoulder', {-180, 0, 90}, {0, 0, 0} )
    jRightElbow     = gr.joint( 'jRightElbow', {-90, 0, 0}, {0, 0, 0} )
    jRightWrist     = gr.joint( 'jRightWrist', {-45, 0, 45}, {0, 0, 0} )
    jLeftHip        = gr.joint( 'jLeftHip', {-60, 0, 60}, {0, 0, 0} )
    jLeftKnee       = gr.joint( 'jLeftKnee', {0, 0, 90}, {0, 0, 0} )
    jLeftAnkle      = gr.joint( 'jLeftAnkle', {0, 0, 45}, {0, 0, 0} )
    jRightHip       = gr.joint( 'jRightHip', {-60, 0, 60}, {0, 0, 0} )
    jRightKnee      = gr.joint('jRightKnee', {0, 0, 90}, {0, 0, 0} )
    jRightAnkle     = gr.joint( 'jRightAnkle', {0, 0, 45}, {0, 0, 0} )
    
    -- Create Geometry nodes
    torso = Torso( 'Torso' )
    shoulders = Shoulders( 'Shoulders' )
    neck = Neck( 'Neck' )
    
    leftUpperArm = UpperArm( 'LeftUpperArm' )
    leftForearm = Forearm( 'LeftForearm' )
    leftHand = Hand( 'LeftHand' )
    
    rightUpperArm = UpperArm( 'RightUpperArm' )
    rightForearm = Forearm( 'RightForearm' )
    rightHand = Hand( 'RightHand' )
    
    hips = Hips( 'Hips' )
    
    leftThigh = Thigh( 'LeftThigh' )
    leftCalf = Calf( 'LeftCalf' )
    leftFoot = Foot( 'LeftFoot' )
    
    rightThigh = Thigh( 'RightThigh' )
    rightCalf = Calf( 'RightCalf' )
    rightFoot = Foot( 'RightFoot' )
    
    head = Head( 'Head' )
    
    -- Construct tree
    jPuppet:add_child( jTorso )
        -- Torso
        jTorso:add_child( torso )
        jTorso:add_child( jShoulders )
            -- Shoulders
            jShoulders:add_child( shoulders )
            jShoulders:add_child( jBaseNeck )
                jBaseNeck:add_child( neck )
                jBaseNeck:add_child( jTopNeck )
                    jTopNeck:add_child( head )
            -- Left Shoulder
            jShoulders:add_child( jLeftShoulder )
                jLeftShoulder:add_child( leftUpperArm )
                jLeftShoulder:add_child( jLeftElbow )
                    jLeftElbow:add_child( leftForearm )
                    jLeftElbow:add_child( jLeftWrist )
                        jLeftWrist:add_child( leftHand )
            -- Right Shoulder
            jShoulders:add_child( jRightShoulder )
                jRightShoulder:add_child( rightUpperArm )
                jRightShoulder:add_child( jRightElbow )
                    jRightElbow:add_child( rightForearm )
                    jRightElbow:add_child( jRightWrist )
                        jRightWrist:add_child( rightHand )
        -- Hips
        jTorso:add_child( jHips )
            -- Left Hip
            jHips:add_child( hips )
            jHips:add_child( jLeftHip )
                jLeftHip:add_child( leftThigh )
                jLeftHip:add_child( jLeftKnee )
                    jLeftKnee:add_child( leftCalf )
                    jLeftKnee:add_child( jLeftAnkle )
                        jLeftAnkle:add_child( leftFoot )
            -- Right Hip
            jHips:add_child( jRightHip )
                jRightHip:add_child( rightThigh )
                jRightHip:add_child( jRightKnee )
                    jRightKnee:add_child( rightCalf )
                    jRightKnee:add_child( jRightAnkle )
                        jRightAnkle:add_child( rightFoot )
    
    jTorso:translate( 0.0, 0.0, 0.0 )
    
    jShoulders:translate( 0.0, 0.9, 0.0 )
    
    jBaseNeck:translate( 0, 0.5, 0 )
    
    jHips:translate( 0.0, -1.1, 0.0 )
    
    jLeftHip:translate( 0.5, -0.5, 0.0 )
    jRightHip:translate( -0.5, -0.5, 0.0 )
    
    jLeftKnee:translate( 0.0, -1.1, 0.0 )
    jRightKnee:translate( 0.0, -1.1, 0.0 )
    
    jLeftAnkle:translate( 0.0, -0.65, 0.15 )
    jRightAnkle:translate( 0.0, -0.65, 0.15 )
    
    jLeftShoulder:translate( 1.2, -0.8, 0.0 )
    jRightShoulder:translate( -1.2, -0.8, 0.0 )
    --jLeftShoulder:rotate( 'z', 180 )
    --jRightShoulder:rotate( 'z', 180 )
    
    jLeftElbow:translate( 0.0, -0.9, 0.0 )
    jRightElbow:translate( 0.0, -0.9, 0.0 )
    
    jLeftWrist:translate( 0.0, -0.65, 0.0 )
    jRightWrist:translate( 0.0, -0.65, 0.0 )

    --jPuppet:translate( -2.50, 0.3, 0 )
    return jPuppet
end

function Create_ball()
    local jBall = gr.joint( 'jBall', {0, 0, 0}, {0, 0, 0} )

    local gBall = gr.mesh( 'uvsphere', 'Soccer Ball' )
    gBall:set_material( orange )
    --gBall:texture( 'Assets/soccer.bmp' )
    gBall:scale( 0.17, 0.17, 0.17 )

    jBall:translate( 0.0, -0.7, 5.0 )
    jBall:add_child( gBall )

    return jBall
end

function Create_net()
    local jNet = gr.joint( 'Soccer Net', {0, 0, 0}, {0, 0, 0} )

    local jPost = gr.joint( 'jTopPost', {0, 0, 0}, {0, 0, 0} )
    --local gPost = gr.mesh( 'uvsphere', 'gTopPost' )
    local gPost = gr.mesh( 'uvcube', 'gTopPost' )
    gPost:set_material( white )
    gPost:scale(3, 0.1, 0.1)
    jPost:add_child( gPost )
    jPost:translate( 0, 1.45, 0 )

    local jLeftPost = gr.joint( 'jLeftPost', {0, 0, 0}, {0, 0, 0} )
    --local gLeftPost = gr.mesh( 'uvsphere', 'gLeftPost')
    local gLeftPost = gr.mesh( 'uvcube', 'gLeftPost')
    gLeftPost:set_material( white )
    gLeftPost:scale(0.1, 1.25, 0.1)
    jLeftPost:add_child( gLeftPost )
    jLeftPost:translate( -2.95, 0.3, 0 )

    local jRightPost = gr.joint( 'jRightPost', {0, 0, 0}, {0, 0, 0} )
    --local gRightPost = gr.mesh( 'uvsphere', 'gRightPost')
    local gRightPost = gr.mesh( 'uvcube', 'gRightPost')
    gRightPost:set_material( white )
    gRightPost:scale(0.1, 1.25, 0.1)
    jRightPost:add_child( gRightPost )
    jRightPost:translate( 2.95, 0.3, 0 )

    jNet:add_child( jPost )
    jNet:add_child( jLeftPost )
    jNet:add_child( jRightPost )

    jNet:translate( 0, 0.1, 0 )
    return jNet
end

function Create_arrow()
    local jArrow = gr.joint( 'Arrow', {0, 0, 90}, {-90, 0, 90} )
    local gPoint = gr.mesh( 'uvsphere', 'Arrow Point' )
    gPoint:set_material( white )
    gPoint:scale( 0.03, 0.03, 0.3 )
    jArrow:add_child( gPoint )

    local jLeft = gr.joint( 'Arrow Left', {0, 0, 0}, {0, 0, 45} )
    local gLeft = gr.mesh( 'uvsphere', 'Arrow Left' )
    gLeft:set_material( white )
    gLeft:scale( 0.12, 0.03, 0.03 )
    jLeft:add_child( gLeft )
    jLeft:translate( -0.08, 0, -0.21 )
    jLeft:rotate( 'y', 45 )

    local jRight = gr.joint( 'Arrow Right', {0, 0, 0}, {-45, 0, 0} )
    local gRight = gr.mesh( 'uvsphere', 'Arrow Right' )
    gRight:set_material( white )
    gRight:scale( 0.12, 0.03, 0.03 )
    jRight:add_child( gRight )
    jRight:translate( 0.08, 0, -0.21 )
    jRight:rotate( 'y', -45 )

    jArrow:translate( 0.0, -0.7, 5.0 )
    jArrow:add_child( jLeft )
    jArrow:add_child( jRight )
    return jArrow
end

function Create_skybox()
    local jSkybox = gr.joint( 'Skybox', {0, 0, 0}, {0, 0, 0} )
    local gSkybox = gr.mesh( 'uvcube', 'Skybox' )
    gSkybox:set_material( blue )
    --gSkybox:scale( 20, 20, 20 )
    --jSkybox:translate( 0, 0, 7 )

    jSkybox:add_child( gSkybox );
    return jSkybox;
end

function Create_field()
    local jPlane = gr.joint( 'Plane', {0, 0, 0}, {0, 0, 0} )
    local gPlane = gr.mesh( 'plane', 'plane1' )
    gPlane:set_material( green )
    gPlane:scale( 5000, 1, 5000)

    jPlane:translate( 0, -0.5, 20 )

    jPlane:add_child( gPlane )
    return jPlane
end

function Create_sky_plane( name, x, y )
    local jSky = gr.joint( name, x, y )
    local gSky = gr.mesh( 'plane', name )

    gSky:set_material( blue )
    if( name == 'Top Sky' )then
        gSky:scale( 422, 1, 422 )
    else
        gSky:scale( 422, 1, 300 )
    end

    return jSky, gSky
end

function Create_sky()
    -- Top Sky
    local jTopSky, gTopSky = Create_sky_plane( 'Top Sky', {0, 0, 180}, {0, 0, 0} )

    jTopSky:translate( 0, 260, 20 )
    jTopSky:rotate( 'z', 180 )
    gTopSky:texture( 'Assets/ypos.png' );
    jTopSky:add_child( gTopSky )

    -- Left Sky
    local jFrontSky, gFrontSky = Create_sky_plane( 'Front Sky', {0, 0, 90}, {0, 0, 0} )

    jFrontSky:translate( 0, 110, -190 )
    jFrontSky:rotate( 'x', 90 )
    gFrontSky:texture( 'Assets/zneg.png' )
    jFrontSky:add_child( gFrontSky )

    local jBackSky, gBackSky = Create_sky_plane( 'Back Sky', {0, 0, 90}, {0, 0, 180} )
    jBackSky:translate( 0, 110, 230 )
    jBackSky:rotate( 'y', 180 )
    jBackSky:rotate( 'x', 90 )
    gBackSky:texture( 'Assets/zpos.png' )
    jBackSky:add_child( gBackSky )

    local jLeftSky, gLeftSky = Create_sky_plane( 'Left Sky', {0, 0, 0}, {0, 0, 90} )
    jLeftSky:translate( -210, 110, 20 )
    jLeftSky:rotate( 'z', -90 )
    jLeftSky:rotate( 'y', 90 )
    gLeftSky:texture( 'Assets/xneg.png' );
    jLeftSky:add_child( gLeftSky )

    local jRightSky, gRightSky = Create_sky_plane( 'Left Sky', {0, 0, 0}, {-90, 0, 0} )
    jRightSky:translate( 210, 110, 20 )
    jRightSky:rotate( 'z', 90 )
    jRightSky:rotate( 'y', -90 )
    gRightSky:texture( 'Assets/xpos.png' );
    jRightSky:add_child( gRightSky )

    local jSky = gr.joint( 'Sky', {0, 0, 0}, {0, 0, 0} )
    jSky:add_child( jTopSky )
    jSky:add_child( jFrontSky )
    jSky:add_child( jBackSky )
    jSky:add_child( jLeftSky )
    jSky:add_child( jRightSky )

    return jSky;
end

function Create_scoreboard()
    local jScoreboard = gr.joint( 'jScoreboard', {0, 0, 0}, {0, 0, 0} )
    local gScoreboard = gr.mesh( 'uvcube', 'gScoreboard' )
    gScoreboard:set_material( grey )
    gScoreboard:scale(4.5, 2.5, 0.1)

    local jBrazil = gr.joint( 'jBrazil', {0, 0, 0}, {0, 0, 0} )
    local gBrazil = gr.mesh( 'plane', 'gBrazil' )
    gBrazil:set_material( white )
    gBrazil:texture( 'Assets/brazil_flag.png' )
    gBrazil:scale(1.5, 1, 1)
    jBrazil:add_child( gBrazil )
    jBrazil:translate( -2, 1, 0.12 )

    local jBTenth = gr.joint( 'jBTenth', {0, 0, 0}, {0, 0, 0} )
    local gBTenth = gr.mesh( 'plane', 'gBTenth' )
    gBTenth:set_material( red )
    gBTenth:texture( 'Assets/0.png' )
    gBTenth:scale( 0.7, 1, 1 )
    jBTenth:add_child( gBTenth )
    jBTenth:translate( -0.7, -2.3, 0.12 )
    jBrazil:add_child( jBTenth )

    local jBUnit = gr.joint( 'jBUnit', {0, 0, 0}, {0, 0, 0} )
    local gBUnit = gr.mesh( 'plane', 'gBUnit' )
    gBUnit:set_material( blue )
    gBUnit:texture( 'Assets/0.png' )
    gBUnit:scale( 0.7, 1, 1 )
    jBUnit:add_child( gBUnit )
    jBUnit:translate( 0.8, -2.3, 0.12 )
    jBrazil:add_child( jBUnit )

    local jEgypt = gr.joint( 'jEgypt', {0, 0, 0}, {0, 0, 0} )
    local gEgypt = gr.mesh( 'plane', 'gEgypt' )
    gEgypt:set_material( white )
    gEgypt:texture( 'Assets/egypt_flag.png' )
    gEgypt:scale( 1.5, 1, 1 )
    jEgypt:add_child( gEgypt )
    jEgypt:translate( 2, 1, 0.12 )

    local jETenth = gr.joint( 'jETenth', {0, 0, 0}, {0, 0, 0} )
    local gETenth = gr.mesh( 'plane', 'gETenth' )
    gETenth:set_material( red )
    gETenth:texture( 'Assets/0.png' )
    gETenth:scale( 0.7, 1, 1 )
    jETenth:add_child( gETenth )
    jETenth:translate( -0.8, -2.3, 0.12 )
    jEgypt:add_child( jETenth )

    local jEUnit = gr.joint( 'jEUnit', {0, 0, 0}, {0, 0, 0} )
    local gEUnit = gr.mesh( 'plane', 'gEUnit' )
    gEUnit:set_material( blue )
    gEUnit:texture( 'Assets/0.png' )
    gEUnit:scale( 0.7, 1, 1 )
    jEUnit:add_child( gEUnit )
    jEUnit:translate( 0.7, -2.3, 0.12 )
    jEgypt:add_child( jEUnit )

    jScoreboard:translate( 0, 7, -10 )

    --jScoreboard:add_child( gScoreboard )
    jScoreboard:add_child( jBrazil )
    jScoreboard:add_child( jEgypt )
    return jScoreboard
end

rootNode = gr.node( 'root' )
jPuppet = Create_goalie()
jBall = Create_ball()
jNet = Create_net()
jArrow = Create_arrow()
jScoreboard = Create_scoreboard()

rootNode:add_child( jBall )
rootNode:add_child( jArrow )
rootNode:add_child( jPuppet )
rootNode:add_child( jNet )
rootNode:add_child( jScoreboard )

return rootNode

