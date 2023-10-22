#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    double x, y, z;
} Vec3;

typedef struct
{
    Vec3 p1, p2, p3;
    Vec3 normal;
} Trian;

typedef struct
{
    int v, vt, vn;
} FaceVertex;

int main()
{
    FILE *file = fopen("file.obj", "r");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open the .obj file.\n");
        return 1;
    }

    char line[128];
    Vec3 *vertices = NULL;
    Vec3 *normals = NULL;
    Vec3 *textures = NULL;
    Trian *triangles = NULL;
    int numVertices = 0;
    int numNormals = 0;
    int numTextures = 0;
    int numTriangles = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            Vec3 vertex;
            if (sscanf(line, "v %lf %lf %lf", &vertex.x, &vertex.y, &vertex.z) == 3)
            {
                numVertices++;
                vertices = (Vec3 *)realloc(vertices, numVertices * sizeof(Vec3));
                vertices[numVertices - 1] = vertex;
            }
        }
        else if (line[0] == 'v' && line[1] == 'n')
        {
            Vec3 normal;
            if (sscanf(line, "vn %lf %lf %lf", &normal.x, &normal.y, &normal.z) == 3)
            {
                numNormals++;
                normals = (Vec3 *)realloc(normals, numNormals * sizeof(Vec3));
                normals[numNormals - 1] = normal;
            }
        }
        else if (line[0] == 'v' && line[1] == 't')
        {
            Vec3 texture;
            if (sscanf(line, "vt %lf %lf %lf", &texture.x, &texture.y, &texture.z) >= 2)
            {
                numTextures++;
                textures = (Vec3 *)realloc(textures, numTextures * sizeof(Vec3));
                textures[numTextures - 1] = texture;
            }
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            FaceVertex face[3];
            if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &face[0].v, &face[0].vt, &face[0].vn,
                       &face[1].v, &face[1].vt, &face[1].vn,
                       &face[2].v, &face[2].vt, &face[2].vn) == 9)
            {
                numTriangles++;
                triangles = (Trian *)realloc(triangles, numTriangles * sizeof(Trian));

                for (int i = 0; i < 3; i++)
                {
                    triangles[numTriangles - 1].normal = normals[face[i].vn - 1];
                    switch (i)
                    {
                    case 0:
                        triangles[numTriangles - 1].p1 = vertices[face[i].v - 1];
                        break;
                    case 1:
                        triangles[numTriangles - 1].p2 = vertices[face[i].v - 1];
                        break;
                    case 2:
                        triangles[numTriangles - 1].p3 = vertices[face[i].v - 1];
                        break;
                    }
                }
            }
        }
    }

    fclose(file);

    if (numVertices == 0 || numTriangles == 0)
    {
        fprintf(stderr, "Error: No vertices or triangles found in the .obj file.\n");
        return 1;
    }

    // Now, you have an array of Trian structures that represent triangles.
    // You can use this data to render the object or perform other operations.

    // Example: Printing triangle vertices and normals
    for (int i = 0; i < numTriangles; i++)
    {
        printf("Triangle %d:\n", i);
        printf("  Vertex 1: x=%.2lf, y=%.2lf, z=%.2lf\n", triangles[i].p1.x, triangles[i].p1.y, triangles[i].p1.z);
        printf("  Vertex 2: x=%.2lf, y=%.2lf, z=%.2lf\n", triangles[i].p2.x, triangles[i].p2.y, triangles[i].p2.z);
        printf("  Vertex 3: x=%.2lf, y=%.2lf, z=%.2lf\n", triangles[i].p3.x, triangles[i].p3.y, triangles[i].p3.z);
        printf("  Normal: x=%.2lf, y=%.2lf, z=%.2lf\n", triangles[i].normal.x, triangles[i].normal.y, triangles[i].normal.z);
    }

    // Remember to free allocated memory

    free(vertices);
    free(normals);
    free(textures);
    free(triangles);

    return 0;
}
