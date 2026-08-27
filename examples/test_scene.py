# Test Scene Script for Maya Real-time Lighting Plugin
# Run this in Maya's Python console to create a test scene

import maya.cmds as cmds
import maya.api.OpenMaya as om

def create_test_scene():
    """
    Creates a simple test scene with:
    - A sphere (with standard material)
    - A cube (with standard material)
    - A ground plane
    - Three lights
    """
    
    print("Creating test scene...")
    
    # Clear existing objects
    cmds.select(all=True)
    cmds.delete()
    
    # Create sphere
    sphere = cmds.polySphere(r=2, sx=32, sy=32, name="TestSphere")
    cmds.move(0, 2, 0, sphere[0])
    cmds.setAttr(sphere[0] + ".displayHandle", True)
    
    # Create cube
    cube = cmds.polyCube(w=2, h=2, d=2, sx=1, sy=1, sz=1, name="TestCube")
    cmds.move(-4, 1, 0, cube[0])
    
    # Create ground plane
    ground = cmds.polyPlane(w=20, h=20, sx=10, sy=10, name="Ground")
    cmds.move(0, 0, 0, ground[0])
    cmds.rotate(0, 0, 0, ground[0])
    
    # Create materials
    # Sphere material (red)
    sphere_mat = cmds.shadingNode('standardSurface', asShader=True, name='SphereMaterial')
    cmds.setAttr(sphere_mat + ".baseColor", 1.0, 0.2, 0.2, type="double3")
    cmds.setAttr(sphere_mat + ".metalness", 0.5)
    cmds.setAttr(sphere_mat + ".roughness", 0.3)
    
    # Cube material (blue)
    cube_mat = cmds.shadingNode('standardSurface', asShader=True, name='CubeMaterial')
    cmds.setAttr(cube_mat + ".baseColor", 0.2, 0.2, 1.0, type="double3")
    cmds.setAttr(cube_mat + ".metalness", 0.1)
    cmds.setAttr(cube_mat + ".roughness", 0.6)
    
    # Ground material (gray)
    ground_mat = cmds.shadingNode('standardSurface', asShader=True, name='GroundMaterial')
    cmds.setAttr(ground_mat + ".baseColor", 0.7, 0.7, 0.7, type="double3")
    cmds.setAttr(ground_mat + ".metalness", 0.0)
    cmds.setAttr(ground_mat + ".roughness", 0.8)
    
    # Assign materials
    cmds.select(sphere[0])
    cmds.hyperShade(assign=sphere_mat)
    
    cmds.select(cube[0])
    cmds.hyperShade(assign=cube_mat)
    
    cmds.select(ground[0])
    cmds.hyperShade(assign=ground_mat)
    
    # Create lights
    # Main light
    light1 = cmds.pointLight(name='MainLight')
    cmds.move(5, 8, 5, light1)
    cmds.setAttr(light1 + ".intensity", 1.5)
    cmds.setAttr(light1 + ".color", 1.0, 1.0, 1.0, type="double3")
    
    # Fill light
    light2 = cmds.pointLight(name='FillLight')
    cmds.move(-5, 4, 3, light2)
    cmds.setAttr(light2 + ".intensity", 0.5)
    cmds.setAttr(light2 + ".color", 0.7, 0.7, 1.0, type="double3")
    
    # Back light
    light3 = cmds.pointLight(name='BackLight')
    cmds.move(0, 3, -8, light3)
    cmds.setAttr(light3 + ".intensity", 0.8)
    cmds.setAttr(light3 + ".color", 1.0, 0.8, 0.6, type="double3")
    
    # Create camera
    camera = cmds.camera(name='TestCamera')
    cmds.move(0, 3, 15, camera[0])
    cmds.rotate(-10, 0, 0, camera[0])
    
    # Set camera as active
    cmds.lookThrough(camera[0])
    
    print("Test scene created successfully!")
    print("Geometry:")
    print(f"  - Sphere at (0, 2, 0)")
    print(f"  - Cube at (-4, 1, 0)")
    print(f"  - Ground plane at (0, 0, 0)")
    print("Lights:")
    print(f"  - Main light at (5, 8, 5)")
    print(f"  - Fill light at (-5, 4, 3)")
    print(f"  - Back light at (0, 3, -8)")
    print("\nTo enable real-time lighting:")
    print("  1. Select any mesh in the viewport")
    print("  2. Run: rtLightingToggle on")

if __name__ == "__main__":
    create_test_scene()
