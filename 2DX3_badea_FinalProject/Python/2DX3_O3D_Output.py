# Abdurahman Bade
# 400247875

import numpy as np
import open3d as o3d

if __name__ == "__main__":
    scans = int(input("Enter Number of Scans:"))
    spins = 16
    print("Read in the prism PCD")
    pcd = o3d.io.read_point_cloud("2DX3_Output.xyz", format="xyz")

    print("The PCD array:")
    print(np.asarray(pcd.points))

    print("Let us visualize the PCD: ")
    o3d.visualization.draw_geometries([pcd])

    
    yz_slice_vertex = []
    for x in range(0,scans*spins):
        yz_slice_vertex.append([x])

    lines = []  
    for x in range(0,scans*spins,spins):
        for i in range(spins):
            if i==spins-1:
                lines.append([yz_slice_vertex[x+i], yz_slice_vertex[x]])
            else:
                lines.append([yz_slice_vertex[x+i], yz_slice_vertex[x+i+1]])
            

    for x in range(0,scans*spins-spins-1,spins):
        for i in range(spins):
            lines.append([yz_slice_vertex[x+i], yz_slice_vertex[x+i+spins]])

    line_set = o3d.geometry.LineSet(points=o3d.utility.Vector3dVector(np.asarray(pcd.points)),lines=o3d.utility.Vector2iVector(lines))

    o3d.visualization.draw_geometries([line_set])
                                    
    
 
