using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Parabox.CSG;
using UnityEditor;
using UnityMeshSimplifier;

namespace KhepriUnity {
    public class Primitives : KhepriBase.Primitives {

        GameObject currentParent;
        List<GameObject> parents;
        public static GameObject selectedGameObject { get; set; }
        Dictionary<GameObject, Color> highlighted;
        Transform playerTransform;
        Camera mainCamera;
        Material currentMaterial;
        Material defaultMaterial;
        bool applyMaterials;
        bool applyColliders;
        bool enableLights;
        bool enablePointlightShadows;
        bool enableMergeParent;
        bool bakedLights;
        bool applyLOD;
        LODLevel[] lodLevels;
        SimplificationOptions lodSimplificationOptions;
        bool makeUVs;
        bool makeStatic;
        Channel channel;
        private Processor<Channel, Primitives> processor;
        private List<GameObject> lights;
        public static bool inSelectionProcess { get; set; }

        public Primitives(GameObject mainObject) {
            this.currentParent = mainObject;
            this.parents = new List<GameObject> { mainObject };
            this.highlighted = new Dictionary<GameObject, Color> { };
            this.mainCamera = Camera.main;
            this.playerTransform = GameObject.FindWithTag("Player").transform;
            this.defaultMaterial = new Material(Shader.Find("Standard"));
            defaultMaterial.enableInstancing = true;
            this.currentMaterial = defaultMaterial;
            this.applyMaterials = true;
            this.applyColliders = true;
            this.enableLights = true;
            this.enablePointlightShadows = true;
            this.bakedLights = false;
            this.applyLOD = true;
            this.makeUVs = false;
            this.makeStatic = true;
            this.enableMergeParent = false;
            inSelectionProcess = false;
            this.lights = new List<GameObject>();
        }
        
        public Primitives(Channel channel, GameObject mainObject) {
            this.channel = channel;
            this.currentParent = mainObject;
            this.parents = new List<GameObject> { mainObject };
            this.highlighted = new Dictionary<GameObject, Color> { };
            this.mainCamera = Camera.main;
            this.playerTransform = GameObject.FindWithTag("Player").transform;
            this.defaultMaterial = new Material(Shader.Find("Standard"));
            defaultMaterial.enableInstancing = true;
            this.currentMaterial = defaultMaterial;
            this.applyMaterials = true;
            this.applyColliders = true;
            this.enableLights = true;
            this.enablePointlightShadows = true;
            this.bakedLights = false;
            this.applyLOD = true;
            this.makeUVs = false;
            this.makeStatic = true;
            this.enableMergeParent = false;
            inSelectionProcess = false;
            this.lights = new List<GameObject>();
        }

        public GameObject CreateParent(String name) {
            GameObject newParent = MakeStatic(new GameObject(name));
            SetActive(newParent, false);
            parents.Add(newParent);
            return newParent;
        }

        public void SetChannel(Channel channel) => this.channel = channel;
        public void SetProcessor(Processor<Channel, Primitives> processor) => this.processor = processor;
        
        public GameObject CurrentParent() => currentParent;
        public GameObject SetCurrentParent(GameObject newParent) {
            GameObject prevParent = currentParent;
            currentParent = newParent;
            return prevParent;
        }
        public void SetActive(GameObject obj, bool state) => obj.SetActive(state);

        public void DeleteMany(GameObject[] objs) {
            int count = objs.Length;
            for (int i = 0; i < count; i++) {
                lights.Remove(objs[0]);
                highlighted.Remove(objs[0]);
                GameObject.DestroyImmediate(objs[0]);
            }
        }
        public void DeleteAllInParent(GameObject parent) {
            int count = parent.transform.childCount;
            for (int i = 0; i < count; i++) {
                lights.Remove(parent.transform.GetChild(0).gameObject);
                highlighted.Remove(parent.transform.GetChild(0).gameObject);
                GameObject.DestroyImmediate(parent.transform.GetChild(0).gameObject);
            }
        }
        public void DeleteAll() {
            foreach (GameObject parent in parents) {
                DeleteAllInParent(parent);
            }
        }
        
        public void SetNonInteractiveRequests() {
            processor.MaxRepeated = Int32.MaxValue;
        }
        
        public void SetInteractiveRequests() {
            processor.MaxRepeated = 0;
        }

        public Material LoadMaterial(String name) => Resources.Load<Material>(name);
        public void SetCurrentMaterial(Material material) => currentMaterial = material;
        public Material CurrentMaterial() => currentMaterial;
        public void SetApplyMaterials(bool apply) => applyMaterials = apply;
        public void SetApplyColliders(bool apply) => applyColliders = apply;
        public void SetApplyLOD(bool apply) => applyLOD = apply;
        public void SetLODLevels(LODLevel[] lodLevels) => this.lodLevels = lodLevels;
        public void SetLODSimplificationOptions(SimplificationOptions options) =>
            this.lodSimplificationOptions = options;
        public void SetEnableMergeParent(bool enable) => enableMergeParent = enable;
        public void SetCalculateUV(bool apply) => makeUVs = apply;

        public GameObject ApplyMaterial(GameObject obj, Material material) {
            Renderer renderer = obj.GetComponent<Renderer>();
            if (applyMaterials) {
                renderer.sharedMaterial = material;
            }
            else {
                renderer.sharedMaterial = defaultMaterial;
            }
            return obj;
        }

        public void SetEnableLights(bool apply) {
            if (apply != enableLights) {
                enableLights = apply;
                // Update all current lights retroatively to enableLights value
                UpdateEnableLights();
            }
        }
        
        public void SetEnablePointLightsShadow(bool apply) {
            if (apply != enablePointlightShadows) {
                enablePointlightShadows = apply;
                // Update all current lights retroatively to enableLights value
                UpdateEnablePointLightsShadow();
            }
        }
        
        public void SetBakedLights(bool apply) {
            if (apply != bakedLights) {
                bakedLights = apply;
                // Update all current lights retroatively to enableLights value
                UpdateBakedLights();
            }
        }

        private void FindPointLights() {
            if (lights.Count == 0) {
                foreach (Transform child in currentParent.transform) {
                    if (child.name == "PointLight")
                        lights.Add(child.gameObject);
                }
            }
        }
        
        private void UpdateEnableLights() {
            FindPointLights();
            foreach (var light in lights) { // FIXME this can be parallelized
                light.GetComponent<Light>().enabled = enableLights; 
            }
        }

        private void UpdateBakedLights() {
            FindPointLights();
            foreach (var light in lights) { 
                light.GetComponent<Light>().lightmapBakeType = bakedLights ? LightmapBakeType.Baked : LightmapBakeType.Realtime;
            }
        }

        private void UpdateEnablePointLightsShadow() {
            FindPointLights();
            foreach (var light in lights) { 
                light.GetComponent<Light>().shadows = enablePointlightShadows ? LightShadows.Hard : LightShadows.None;
            }
        }

        public GameObject ApplyCurrentMaterial(GameObject obj) =>
            ApplyMaterial(obj, currentMaterial);

        public GameObject LoadResource(String name) => Resources.Load<GameObject>(name);

        public GameObject PointLight(Vector3 position, Color color, float range, float intensity) {
            GameObject pLight = new GameObject("PointLight");
            Light light = pLight.AddComponent<Light>();
            light.enabled = enableLights;
            pLight.transform.parent = currentParent.transform;
            light.type = LightType.Point;
            light.color = color;
            light.range = range;         // How far the light is emitted from the center of the object
            light.intensity = intensity; // Brightness of the light
            light.shadows = enablePointlightShadows ? LightShadows.Hard : LightShadows.None;
            light.lightmapBakeType = bakedLights ? LightmapBakeType.Baked : LightmapBakeType.Realtime;
            pLight.transform.localPosition = position;
            lights.Add(pLight);
            return MakeStatic(pLight);
        }

        public GameObject Window(Vector3 position, Quaternion rotation, float dx, float dy, float dz) {
            GameObject s = GameObject.CreatePrimitive(PrimitiveType.Cube);
            s.name = "Window";
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(Math.Abs(dx), Math.Abs(dy), Math.Abs(dz));
            s.transform.localRotation = rotation;
            s.transform.localPosition = position + rotation * new Vector3(dx / 2, dy / 2, dz / 2);
            ApplyMaterial(s, Resources.Load<Material>("Materials/Glass/Glass"));
            ApplyCollider(s);
            return MakeStatic(s);
        }

        /*
                static Vector3 vpol(float rho, float phi) => new Vector3(rho * Mathf.Cos(phi), rho * Mathf.Sin(phi), 0);

                public List<GameObject> RowOfGameObjects(Vector3 c, float angle, int n, float spacing, GameObject family) {
                    Quaternion rot = Quaternion.Euler(0, 0, Mathf.Rad2Deg*angle + 90);
                    return Enumerable.Range(0, n).Select(i => TransformedGameObject(family, c + vpol(spacing * i, angle), rot)).ToList();
                }

                public List<GameObject> CenteredRowOfGameObjects(Vector3 c, float angle, int n, float spacing, GameObject family) =>
                    RowOfGameObjects(c + vpol(-spacing * (n - 1) / 2, angle), angle, n, spacing, family);

                // BIM Table
                public List<GameObject> BaseRectangularTable(float length, float width, float height, float top_thickness, float leg_thickness) {
                    List<GameObject> objs = new List<GameObject>();
                    GameObject table = new Solid3d();
                    table.CreateBox(length, width, top_thickness);
                    table.TransformBy(Quaternion.Displacement(new Vector3d(0, 0, height - top_thickness / 2)));
                    objs.Add(table);
                    float dx = length / 2;
                    float dy = width / 2;
                    float leg_x = dx - leg_thickness / 2;
                    float leg_y = dy - leg_thickness / 2;
                    Vector3[] pts = new Vector3[] {
                            new Vector3(+leg_x, -leg_y, 0),
                            new Vector3(+leg_x, +leg_y, 0),
                            new Vector3(-leg_x, +leg_y, 0),
                            new Vector3(-leg_x, -leg_y, 0)
                        };
                    foreach (Vector3 p in pts) {
                        Solid3d leg = new Solid3d();
                        leg.CreateBox(leg_thickness, leg_thickness, height - top_thickness);
                        leg.TransformBy(Quaternion.Displacement(p - Vector3.Origin + new Vector3d(0, 0, (height - top_thickness) / 2)));
                        objs.Add(leg);
                    }
                    return objs;
                }
                public GameObject CreateRectangularTableFamily(float length, float width, float height, float top_thickness, float leg_thickness) =>
                    CreateBlockFromFunc("Khepri Table", () => BaseRectangularTable(length, width, height, top_thickness, leg_thickness));


                public GameObject Table(Vector3 c, float angle, GameObject family) =>
                    CreateBlockInstanceAtRotated(family, c, angle);

                // BIM Chair block
                public List<GameObject> BaseChair(float length, float width, float height, float seat_height, float thickness) {
                    List<GameObject> objs = BaseRectangularTable(length, width, seat_height, thickness, thickness);
                    float vx = length / 2;
                    float vy = width / 2;
                    float vz = height;
                    Solid3d back = new Solid3d();
                    back.CreateBox(thickness, width, height - seat_height);
                    back.TransformBy(Quaternion.Displacement(new Vector3d((thickness - length) / 2, 0, (seat_height + height) / 2)));
                    objs.Add(back);
                    return objs;
                }
                public GameObject CreateChairFamily(float length, float width, float height, float seat_height, float thickness) =>
                    CreateBlockFromFunc("Khepri Chair", () => BaseChair(length, width, height, seat_height, thickness));

                public GameObject Chair(Vector3 c, float angle, GameObject family) =>
                    CreateBlockInstance(family, new Frame3d(c, vpol(1, angle), vpol(1, angle + Math.PI / 2)));

                // BIM Table and chairs block
                public List<GameObject> BaseRectangularTableAndChairs(GameObject tableFamily, GameObject chairFamily, float tableLength, float tableWidth, int chairsOnTop, int chairsOnBottom, int chairsOnRight, int chairsOnLeft, float spacing) {
                    List<GameObject> objs = new List<GameObject>();
                    float dx = tableLength / 2;
                    float dy = tableWidth / 2;
                    objs.Add(new GameObject(new Vector3(0, 0, 0), tableFamily));
                    objs.AddRange(CenteredRowOfGameObjects(new Vector3(-dx, 0, 0), -Math.PI / 2, chairsOnBottom, spacing, chairFamily));
                    objs.AddRange(CenteredRowOfGameObjects(new Vector3(+dx, 0, 0), +Math.PI / 2, chairsOnTop, spacing, chairFamily));
                    objs.AddRange(CenteredRowOfGameObjects(new Vector3(0, +dy, 0), -Math.PI, chairsOnRight, spacing, chairFamily));
                    objs.AddRange(CenteredRowOfGameObjects(new Vector3(0, -dy, 0), 0, chairsOnLeft, spacing, chairFamily));
                    return objs;
                }

                public GameObject CreateRectangularTableAndChairsFamily(GameObject tableFamily, GameObject chairFamily, float tableLength, float tableWidth, int chairsOnTop, int chairsOnBottom, int chairsOnRight, int chairsOnLeft, float spacing) =>
                    CreateBlockFromFunc("Khepri Table&Chair", () => BaseRectangularTableAndChairs(
                        tableFamily, chairFamily, tableLength, tableWidth,
                        chairsOnTop, chairsOnBottom, chairsOnRight, chairsOnLeft,
                        spacing));

                public GameObject TableAndChairs(Vector3 c, float angle, GameObject family) =>
                    CreateBlockInstanceAtRotated(family, c, angle);

            */

        public GameObject Box(Vector3 position, Vector3 vx, Vector3 vy, float dx, float dy, float dz) {
            Quaternion rotation = Quaternion.LookRotation(vx, vy);
            GameObject s = GameObject.CreatePrimitive(PrimitiveType.Cube);
            s.name = "Box";
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(Math.Abs(dx), Math.Abs(dy), Math.Abs(dz));
            s.transform.localRotation = rotation;
            s.transform.localPosition = position + rotation * new Vector3(dx / 2, dy / 2, dz / 2);
            ApplyCurrentMaterial(s);
            ApplyCollider(s);
            return MakeStatic(s);
        }

        public GameObject Box2(Vector3 position, Vector3 vx, Vector3 vy, float dx, float dy, float dz) {
            Quaternion rotation = Quaternion.LookRotation(vx, vy);
            GameObject box = new GameObject("Box");
            box.transform.parent = GameObject.Find("MainObject").transform;

            MeshRenderer meshRenderer = box.AddComponent<MeshRenderer>();
            MeshFilter meshFilter = box.AddComponent<MeshFilter>();
            Mesh mesh = new Mesh();
            meshFilter.sharedMesh = mesh;
            mesh.vertices = new Vector3[] {
                // face 1 (xy plane)
                new Vector3(position.x,      position.y,      position.z),
                new Vector3(position.x + dx, position.y,      position.z),
                new Vector3(position.x + dx, position.y + dy, position.z),
                new Vector3(position.x,      position.y + dy, position.z), 
                // face 2 (zy plane)
                new Vector3(position.x + dx, position.y,      position.z),
                new Vector3(position.x + dx, position.y,      position.z + dz),
                new Vector3(position.x + dx, position.y + dy, position.z + dz),
                new Vector3(position.x + dx, position.y + dy, position.z), 
                // face 3 (xy plane)
                new Vector3(position.x + dx, position.y,      position.z + dz),
                new Vector3(position.x,      position.y,      position.z + dz),
                new Vector3(position.x,      position.y + dy, position.z + dz),
                new Vector3(position.x + dx, position.y + dy, position.z + dz), 
                // face 4 (zy plane)
                new Vector3(position.x,      position.y,      position.z + dz),
                new Vector3(position.x,      position.y,      position.z),
                new Vector3(position.x,      position.y + dy, position.z),
                new Vector3(position.x,      position.y + dy, position.z + dz), 
                // face 5  (zx plane)
                new Vector3(position.x,      position.y + dy, position.z),
                new Vector3(position.x + dx, position.y + dy, position.z),
                new Vector3(position.x + dx, position.y + dy, position.z + dz),
                new Vector3(position.x,      position.y + dy, position.z + dz), 
                // face 6 (zx plane)
                new Vector3(position.x,      position.y,      position.z),
                new Vector3(position.x,      position.y,      position.z + dz),
                new Vector3(position.x + dx, position.y,      position.z + dz),
                new Vector3(position.x + dx, position.y,      position.z),
            };

            int faces = 6;
            mesh.subMeshCount = faces; // Specify how many submeshes to apply different materials
            List<int> triangles = new List<int>();
            List<Vector2> uvs = new List<Vector2>();

            for (int i = 0; i < faces; i++) {
                int triangleOffset = i * 4;
                triangles.Add(0 + triangleOffset);
                triangles.Add(2 + triangleOffset);
                triangles.Add(1 + triangleOffset);

                triangles.Add(0 + triangleOffset);
                triangles.Add(3 + triangleOffset);
                triangles.Add(2 + triangleOffset);

                // Same uvs for all faces
                uvs.Add(new Vector2(0, 0));
                uvs.Add(new Vector2(1, 0));
                uvs.Add(new Vector2(1, 1));
                uvs.Add(new Vector2(0, 1));
                //             mesh.SetTriangles(triangles.ToArray(), i);
                //             triangles.Clear();
            }
            mesh.uv = uvs.ToArray();
            mesh.triangles = triangles.ToArray();
            //Material material = new Material(Shader.Find("Diffuse")); REMOVE ME

            // Codigo a alterar no set material, fazer um ciclo por cada material

            //            foreach (Material material in renderer.materials)
            //                material.mainTextureScale = new Vector2(scaleX, scaleY);


            //            Material material = currentMaterial;
            //            Material[] mats = { material, material, material, material, material, material }; // This list has the size of #faces or #submeshes, in this case 6
            //            meshRenderer.materials = mats;
            mesh.RecalculateNormals();
            mesh.RecalculateBounds();

            //box.AddComponent<BoxCollider>();
            box.transform.localRotation = rotation;
            ApplyMaterial(box, currentMaterial);
            ApplyCollider(box);
            return MakeStatic(box);
        }

        public GameObject CenteredBox2(Vector3 position, Quaternion rotation, float dx, float dy, float dz, float angle) {
            GameObject box = new GameObject("CenteredBox");
            box.transform.parent = GameObject.Find("MainObject").transform;

            MeshRenderer meshRenderer = box.AddComponent<MeshRenderer>();
            MeshFilter meshFilter = box.AddComponent<MeshFilter>();
            Mesh mesh = new Mesh();
            meshFilter.sharedMesh = mesh;
            mesh.vertices = new Vector3[] {
                // face 1 (xy plane)
                new Vector3(position.x - dx / 2, position.y,      position.z - dz / 2),
                new Vector3(position.x + dx / 2, position.y,      position.z - dz / 2),
                new Vector3(position.x + dx / 2, position.y + dy, position.z - dz / 2),
                new Vector3(position.x - dx / 2, position.y + dy, position.z - dz / 2), 
                // face 2 (zy plane)
                new Vector3(position.x + dx / 2, position.y,      position.z - dz / 2),
                new Vector3(position.x + dx / 2, position.y,      position.z + dz / 2),
                new Vector3(position.x + dx / 2, position.y + dy, position.z + dz / 2),
                new Vector3(position.x + dx / 2, position.y + dy, position.z - dz / 2), 
                // face 3 (xy plane)
                new Vector3(position.x + dx / 2, position.y,      position.z + dz / 2),
                new Vector3(position.x - dx / 2, position.y,      position.z + dz / 2),
                new Vector3(position.x - dx / 2, position.y + dy, position.z + dz / 2),
                new Vector3(position.x + dx / 2, position.y + dy, position.z + dz / 2), 
                // face 4 (zy plane)
                new Vector3(position.x - dx / 2, position.y,      position.z + dz / 2),
                new Vector3(position.x - dx / 2, position.y,      position.z - dz / 2),
                new Vector3(position.x - dx / 2, position.y + dy, position.z - dz / 2),
                new Vector3(position.x - dx / 2, position.y + dy, position.z + dz / 2), 
                // face 5  (zx plane)
                new Vector3(position.x - dx / 2, position.y + dy, position.z - dz / 2),
                new Vector3(position.x + dx / 2, position.y + dy, position.z - dz / 2),
                new Vector3(position.x + dx / 2, position.y + dy, position.z + dz / 2),
                new Vector3(position.x - dx / 2, position.y + dy, position.z + dz / 2), 
                // face 6 (zx plane)
                new Vector3(position.x - dx / 2, position.y,      position.z - dz / 2),
                new Vector3(position.x - dx / 2, position.y,      position.z + dz / 2),
                new Vector3(position.x + dx / 2, position.y,      position.z + dz / 2),
                new Vector3(position.x + dx / 2, position.y,      position.z - dz / 2),
            };

            int faces = 6;
            mesh.subMeshCount = faces; // Specify how many submeshes to apply different materials
            List<int> triangles = new List<int>();
            List<Vector2> uvs = new List<Vector2>();

            for (int i = 0; i < faces; i++) {
                int triangleOffset = i * 4;
                triangles.Add(0 + triangleOffset);
                triangles.Add(2 + triangleOffset);
                triangles.Add(1 + triangleOffset);

                triangles.Add(0 + triangleOffset);
                triangles.Add(3 + triangleOffset);
                triangles.Add(2 + triangleOffset);

                // Same uvs for all faces
                uvs.Add(new Vector2(0, 0));
                uvs.Add(new Vector2(1, 0));
                uvs.Add(new Vector2(1, 1));
                uvs.Add(new Vector2(0, 1));
                mesh.SetTriangles(triangles.ToArray(), i); // Setup submeshes triangles
                triangles.Clear();
            }
            mesh.uv = uvs.ToArray();

            //Material material = new Material(Shader.Find("Diffuse")); REMOVE ME
            Material material = Resources.Load<Material>("Materials/Concrete"); // FIX ME, HARDCODED MATERIAL
            Material[] mats = { material, material, material, material, material, material }; // This list has the size of #faces or #submeshes, in this case 6
            meshRenderer.materials = mats;
            mesh.RecalculateNormals();
            mesh.RecalculateBounds();

            box.AddComponent<BoxCollider>();
            box.transform.localRotation = Quaternion.Euler(0, Mathf.Rad2Deg * angle, 0) * rotation;
            ApplyCollider(box);
            return MakeStatic(box);
        }

        public GameObject RightCuboidNamed(String name, Vector3 position, Vector3 vx, Vector3 vy, float dx, float dy, float dz, float angle, Material material) {
            Quaternion rotation = Quaternion.LookRotation(vx, vy);
            GameObject s = GameObject.CreatePrimitive(PrimitiveType.Cube);
            rotation = rotation * Quaternion.Euler(0, 0, Mathf.Rad2Deg * angle);
            s.name = name;
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(Math.Abs(dx), Math.Abs(dy), Math.Abs(dz));
            s.transform.localRotation = rotation;
            s.transform.localPosition = position + rotation * new Vector3(0, 0, dz / 2);
            ApplyCollider(s);
            ApplyMaterial(s, material);
            return MakeStatic(s);
        }

        /*public GameObject RightCuboid(Vector3 position, Vector3 vx, Vector3 vy, float dx, float dy, float dz, float angle) =>
            ApplyCurrentMaterial(RightCuboidNamed("RightCuboid", position, vx, vy, dx, dy, dz, angle));*/
        
        public GameObject RightCuboid(Vector3 position, Vector3 vx, Vector3 vy, float dx, float dy, float dz, float angle) {
            Quaternion rotation = Quaternion.LookRotation(vx, vy);
            GameObject s = GameObject.CreatePrimitive(PrimitiveType.Cube);
            rotation = rotation * Quaternion.Euler(0, 0, Mathf.Rad2Deg * angle);
            s.name = "RightCuboid";
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(Math.Abs(dx), Math.Abs(dy), Math.Abs(dz));
            s.transform.localRotation = rotation;
            s.transform.localPosition = position + rotation * new Vector3(0, 0, dz / 2);
            ApplyCollider(s);
            ApplyCurrentMaterial(s);
            return MakeStatic(s);
        }

        /*public GameObject SphereNamed(String name, Vector3 center, float radius) {
            GameObject s = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            s.name = name;
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(2 * radius, 2 * radius, 2 * radius);
            s.transform.localPosition = center;
            ApplyCollider(s);
            ApplyLOD(s);
            return MakeStatic(s);
        }*/

        /*public GameObject Sphere(Vector3 center, float radius) =>
            ApplyCurrentMaterial(SphereNamed("Sphere", center, radius));*/
        
        public GameObject Sphere(Vector3 center, float radius) {
            GameObject s = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(2 * radius, 2 * radius, 2 * radius);
            s.transform.localPosition = center;
            ApplyCollider(s);
            ApplyCurrentMaterial(s);
            ApplyLOD(s);
            return MakeStatic(s);
        }

        public GameObject CylinderNamed(String name, Vector3 bottom, float radius, Vector3 top, Material material) {
            GameObject s = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            s.name = name;
            float d = Vector3.Distance(bottom, top);
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(2 * radius, d / 2, 2 * radius);
            s.transform.localRotation = Quaternion.FromToRotation(Vector3.up, top - bottom);
            s.transform.localPosition = bottom + (top - bottom) / 2;
            ApplyCollider(s);
            ApplyMaterial(s, material);
            ApplyLOD(s);
            return MakeStatic(s);
        }

        /*public GameObject Cylinder(Vector3 bottom, float radius, Vector3 top) =>
            ApplyCurrentMaterial(CylinderNamed("Cylinder", bottom, radius, top));*/
        
        public GameObject Cylinder(Vector3 bottom, float radius, Vector3 top) {
            GameObject s = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            float d = Vector3.Distance(bottom, top);
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(2 * radius, d / 2, 2 * radius);
            s.transform.localRotation = Quaternion.FromToRotation(Vector3.up, top - bottom);
            s.transform.localPosition = bottom + (top - bottom) / 2;
            ApplyCollider(s);
            ApplyCurrentMaterial(s);
            ApplyLOD(s);
            return MakeStatic(s);
        }

        Vector3 PlaneNormal(Vector3[] pts) {
            Vector3 pt = pts[0];
            Vector3 sum = Vector3.zero;
            for (int i = 1; i < pts.Length - 1; i++) {
                if (pts[i] == pt || pts[i + 1] == pt) continue;
                sum += Vector3.Cross(pts[i] - pt, pts[i + 1] - pt);
            }
            sum.Normalize();
            return sum;
        }

        Vector3[] ReverseIfNeeded(Vector3[] pts, Vector3 normal) {
            Vector3 normalPts = PlaneNormal(pts);
            return (Vector3.Dot(normalPts, normal) > 0) ? pts : pts.Reverse().ToArray();
        }
        
        Mesh CreatePolygonMesh(Vector3[] ps) {
            Poly2Mesh.Polygon polygon = new Poly2Mesh.Polygon();
            polygon.outside = new List<Vector3>(ps);
            return Poly2Mesh.CreateMesh(polygon);
        }
        
        Mesh CreatePolygonMeshWithHoles(Vector3[] ps, Vector3[][] holes) {
        
            Poly2Mesh.Polygon polygon = new Poly2Mesh.Polygon();
            polygon.outside = new List<Vector3>(ps);
            polygon.holes =
                new List<List<Vector3>>(
                    new List<Vector3[]>(holes).Select(e => new List<Vector3>(e)));
            return Poly2Mesh.CreateMesh(polygon);
        }
        
        Mesh CreateTrigMesh(Vector3[] ps, Vector3 q) {
            Vector3[] vertices = new Vector3[ps.Length + 1];
            Array.Copy(ps, vertices, ps.Length);
            vertices[ps.Length] = q;
            int[] triangles = new int[ps.Length * 3];
            int k = 0;
            for (int i = 0; i < ps.Length - 1; i++) {
                triangles[k++] = i;
                triangles[k++] = i + 1;
                triangles[k++] = ps.Length;
            }
            triangles[k++] = ps.Length - 1;
            triangles[k++] = 0;
            triangles[k++] = ps.Length;
            Mesh mesh = new Mesh();
            mesh.vertices = vertices;
            mesh.triangles = triangles;
            mesh.RecalculateNormals();
            mesh.RecalculateBounds();
            //Unwrapping.GenerateSecondaryUVSet(mesh); 
            return mesh;
        }
        
        Mesh CreateQuadMesh(Vector3[] ps, Vector3[] qs) {
            Vector3[] vertices = new Vector3[ps.Length * 2];
            Array.Copy(ps, vertices, ps.Length);
            Array.Copy(qs, 0, vertices, ps.Length, qs.Length);
            int[] triangles = new int[ps.Length * 2 * 3];
            int k = 0;
            for (int i = 0, j = ps.Length; i < ps.Length - 1; i++, j++) {
                triangles[k++] = i;
                triangles[k++] = i + 1;
                triangles[k++] = j + 1;
                triangles[k++] = i;
                triangles[k++] = j + 1;
                triangles[k++] = j;
            }
            triangles[k++] = ps.Length - 1;
            triangles[k++] = 0;
            triangles[k++] = ps.Length;
            triangles[k++] = ps.Length - 1;
            triangles[k++] = ps.Length;
            triangles[k++] = 2 * ps.Length - 1;
            Mesh mesh = new Mesh();
            mesh.vertices = vertices;
            mesh.triangles = triangles;
            mesh.RecalculateNormals();
            mesh.RecalculateBounds();
            //Unwrapping.GenerateSecondaryUVSet(mesh); 
            return mesh;
        }

        /*GameObject AddQuadMeshAux(GameObject parent, Vector3[] ps, Vector3[] qs, Material material) {
            GameObject s = new GameObject("QuadMesh");
            s.transform.parent = parent.transform;
            MeshRenderer render = s.AddComponent<MeshRenderer>();
            MeshFilter filter = s.AddComponent<MeshFilter>();
            Vector3[] vertices = new Vector3[ps.Length * 2];
            Array.Copy(ps, vertices, ps.Length);
            Array.Copy(qs, 0, vertices, ps.Length, qs.Length);
            Mesh mesh = new Mesh();
            mesh.vertices = vertices;
            List<Vector2> uvs = new List<Vector2>();
            List<Material> materials = new List<Material>();
            for (int i = 0, j = ps.Length; i < ps.Length - 1; i++, j++) {
                int[] triangles = new int[6];
                int k = 0;
                triangles[k++] = i;
                triangles[k++] = i + 1;
                triangles[k++] = j + 1;
                triangles[k++] = i;
                triangles[k++] = j + 1;
                triangles[k++] = j;
                // Same uvs for all faces
                uvs.Add(new Vector2(0, 0));
                uvs.Add(new Vector2(1, 0));
                uvs.Add(new Vector2(1, 1));
                uvs.Add(new Vector2(0, 1));
                Material mat = GameObject.Instantiate(material);
                mat.mainTextureScale = new Vector2(
                    Vector3.Distance(ps[i], ps[i + 1]),
                    Vector3.Distance(ps[i], qs[i]));
                materials.Add(mat);
                mesh.SetTriangles(triangles.ToArray(), i); // Setup submeshes triangles
            }
            mesh.uv = uvs.ToArray();
            render.materials = materials.ToArray();
            mesh.RecalculateNormals();
            mesh.RecalculateBounds();
            filter.sharedMesh = mesh;
            ApplyCollider(s, mesh);
            return MakeStatic(s);
        }*/

        GameObject ApplyCollider(GameObject obj, Mesh mesh) {
            if (applyColliders) {
                MeshCollider meshCollider = obj.GetComponent<MeshCollider>();
                if (meshCollider == null)
                    meshCollider = obj.AddComponent<MeshCollider>();
                
                meshCollider.sharedMesh = mesh;
            }
            return obj;
        }

        GameObject ApplyCollider(GameObject obj) {
            if (applyColliders) {
                Collider collider = obj.GetComponent<Collider>();
                if (collider == null) {
                    MeshCollider meshCollider = obj.AddComponent<MeshCollider>();
                    meshCollider.sharedMesh = obj.GetComponent<MeshFilter>().sharedMesh;
                }
            }
            else {
                Collider collider = obj.GetComponent<Collider>();
                if (collider != null) {
                    Component.DestroyImmediate(collider);
                }
            }

            return obj;
        }

        /*public GameObject SurfacePolygon(Vector3[] ps) =>
            AddPolygonMesh(currentParent, ReverseIfNeeded(ps, Vector3.up), currentMaterial);*/

        public GameObject PyramidWithMaterial(Vector3[] ps, Vector3 q, Material material) {
            ps = ReverseIfNeeded(ps, Vector3.down);
            GameObject s = new GameObject("Pyramid");
            s.transform.parent = currentParent.transform;
            
            Mesh botMesh = CreatePolygonMesh(ps);
            Array.Reverse(ps);
            Mesh exteriorMesh = CreateTrigMesh(ps, q);
            MeshFilter meshFilter = s.AddComponent<MeshFilter>();
            Mesh[] allMeshes = {botMesh, exteriorMesh};
            meshFilter.sharedMesh = CombineMesh(allMeshes);
            s.AddComponent<MeshRenderer>();
            
            ApplyMaterial(s, material);
            ApplyCollider(s);
            ApplyLOD(s);
            return MakeStatic(s);
        }

        public GameObject Pyramid(Vector3[] ps, Vector3 q) => PyramidWithMaterial(ps, q, currentMaterial);

        public GameObject PyramidFrustumWithMaterial(Vector3[] ps, Vector3[] qs, Material material) {
            ps = ReverseIfNeeded(ps, Vector3.down);
            qs = ReverseIfNeeded(qs, Vector3.up);
            GameObject s = new GameObject("PyramidFrustum");
            s.transform.parent = currentParent.transform;
            //Array.Reverse(ps);
            Mesh botMesh = CreatePolygonMesh(ps);

            Mesh topMesh = CreatePolygonMesh(qs);
            
            Array.Reverse(ps);
            Mesh exteriorMesh = CreateQuadMesh(ps, qs);
            
            MeshFilter meshFilter = s.AddComponent<MeshFilter>();
            Mesh[] allMeshes = {botMesh, topMesh, exteriorMesh};
            meshFilter.sharedMesh = CombineMesh(allMeshes);
            s.AddComponent<MeshRenderer>();
            
            ApplyMaterial(s, material);
            ApplyCollider(s);
            ApplyLOD(s);
            return MakeStatic(s);
        }

        public GameObject PyramidFrustum(Vector3[] ps, Vector3[] qs) =>
            PyramidFrustumWithMaterial(ps, qs, currentMaterial);

        public GameObject ExtrudeContour(Vector3[] contour, Vector3[][] holes, Vector3 v, Material material) {
            contour = ReverseIfNeeded(contour, Vector3.down);
            GameObject s = new GameObject("Slab");
            s.transform.parent = currentParent.transform;
            
            Mesh botMesh = CreatePolygonMeshWithHoles(contour.ToArray(), holes);
            
            Mesh topMesh = new Mesh();
            topMesh.vertices = botMesh.vertices.Select(e => e + v).ToArray();
            topMesh.triangles = botMesh.triangles.Reverse().ToArray();
            topMesh.RecalculateNormals();
            topMesh.RecalculateBounds();
            
            Vector3[] topContour = contour.Select(e => e + v).ToArray();
            Mesh exteriorMesh = CreateQuadMesh(topContour, contour);

            List<Mesh> holeMeshes = new List<Mesh>();
            foreach (Vector3[] hole in holes) {
                Vector3[] topHole = hole.Select(e => e + v).ToArray();
                holeMeshes.Add(CreateQuadMesh(hole.ToArray(), topHole));
            }

            List<Mesh> allMeshes = new List<Mesh>(holeMeshes);
            allMeshes.Add(topMesh);
            allMeshes.Add(botMesh);
            allMeshes.Add(exteriorMesh);
            
            MeshFilter meshFilter = s.AddComponent<MeshFilter>();
            meshFilter.sharedMesh = CombineMesh(allMeshes.ToArray());
            s.AddComponent<MeshRenderer>();
            
            ApplyMaterial(s, material);
            ApplyCollider(s);
            ApplyLOD(s);
            return MakeStatic(s);
        }

        public GameObject SurfaceFromGridWithMaterial(int m, int n, Vector3[] pts, bool closedM, bool closedN, int level, Material material) {
            GameObject s = new GameObject("SurfaceGrid");
            s.transform.parent = currentParent.transform;
            s.AddComponent<MeshRenderer>();
            MeshFilter filter = s.AddComponent<MeshFilter>();
            Vector3[] vertices = pts;
            int[] triangles = new int[pts.Length * 2 * 3];
            int k = 0;
            int rm = closedM ? m : m - 1;
            int rn = closedN ? n : n - 1;
            for (int i = 0; i < rm; i++) {
                for (int j = 0; j < rn; j++) {
                    int i11 = i * n + j;
                    int i12 = i * n + (j + 1) % n;
                    int i22 = ((i + 1) % m) * n + (j + 1) % n;
                    int i21 = ((i + 1) % m) * n + j;
                    triangles[k++] = i11;
                    triangles[k++] = i22;
                    triangles[k++] = i12;
                    triangles[k++] = i11;
                    triangles[k++] = i21;
                    triangles[k++] = i22;
                }
            }
            Mesh mesh = new Mesh();
            mesh.vertices = vertices;
            mesh.triangles = triangles;
            mesh.RecalculateNormals();
            mesh.RecalculateBounds();
            mesh.Optimize();
            //Unwrapping.GenerateSecondaryUVSet(mesh); 
            filter.sharedMesh = mesh;
            ApplyMaterial(s, material);
            ApplyCollider(s, mesh);
            ApplyLOD(s);
            return MakeStatic(s);
        }

        public GameObject SurfaceFromGrid(int m, int n, Vector3[] pts, bool closedM, bool closedN, int level) =>
            SurfaceFromGridWithMaterial(m, n, pts, closedM, closedN, level, currentMaterial);

        // Text
        
        public GameObject Text(string txt, Vector3 pos, Vector3 vx, Vector3 vy, string fontName, int fontSize) {
            GameObject s = new GameObject("Text");
            TextMesh textMesh = s.AddComponent<TextMesh>();
            textMesh.anchor = TextAnchor.LowerLeft;
            //            textMesh.font = (Font)Resources.GetBuiltinResource(typeof(Font), fontName);
            textMesh.font = Resources.Load<Font>(fontName);
            textMesh.fontSize = 100 * fontSize;
            textMesh.text = txt;
            s.transform.parent = currentParent.transform;
            s.transform.localScale = new Vector3(0.01f, 0.01f, 0.01f);
            s.transform.localRotation = Quaternion.LookRotation(vx, vy);
            s.transform.localPosition = pos;
            return s;
        }

        // Transformations

        public void Move(GameObject s, Vector3 v) {
            s.transform.localPosition += v;
        }

        public void Scale(GameObject s, Vector3 p, float scale) {
            Vector3 sp = s.transform.localPosition;
            s.transform.localScale *= scale;
            s.transform.localPosition = p + (sp - p) * scale;
        }

        public void Rotate(GameObject s, Vector3 p, Vector3 n, float a) {
            Vector3 pw = s.transform.parent.TransformPoint(p);
            Vector3 nw = s.transform.parent.TransformVector(n);
            s.transform.RotateAround(pw, nw, -a * Mathf.Rad2Deg);
        }

        // Mesh Combination

        public GameObject Canonicalize(GameObject s) {
            MeshFilter[] meshFilters = s.GetComponentsInChildren<MeshFilter>();
            if (meshFilters.Length == 0) {
                return s;
            }
            Dictionary<Material, List<MeshFilter>> materialMeshFilters = new Dictionary<Material, List<MeshFilter>>();
            foreach (MeshFilter meshFilter in meshFilters) {
                Material[] materials = meshFilter.GetComponent<MeshRenderer>().sharedMaterials;
                if (materials != null) {
                    if (materials.Length > 1 || materials[0] == null) {
                        return s;
                    } else if (materialMeshFilters.ContainsKey(materials[0])) {
                        materialMeshFilters[materials[0]].Add(meshFilter);
                    } else {
                        materialMeshFilters.Add(materials[0], new List<MeshFilter>() { meshFilter });
                    }
                }
            }
            if (materialMeshFilters.Count == 0) {
                return s;
            } else {
                List<GameObject> combinedObjects = new List<GameObject>();
                foreach (KeyValuePair<Material, List<MeshFilter>> entry in materialMeshFilters) {
                    Material material = entry.Key;
                    List<MeshFilter> meshes = entry.Value;
                    string materialName = material.name; // ToString().Split(' ')[0];
                    CombineInstance[] combine = new CombineInstance[meshes.Count];
                    for (int i = 0; i < meshes.Count; i++) {
                        combine[i].mesh = meshes[i].sharedMesh;
                        combine[i].transform = meshes[i].transform.localToWorldMatrix;
                    }
                    Mesh combinedMesh = new Mesh();
                    combinedMesh.CombineMeshes(combine);
                    combinedMesh.Optimize();
                    //Unwrapping.GenerateSecondaryUVSet(combinedMesh); 
                    GameObject combinedObject = new GameObject(materialName);
                    MeshFilter filter = combinedObject.AddComponent<MeshFilter>();
                    filter.sharedMesh = combinedMesh;
                    MeshRenderer renderer = combinedObject.AddComponent<MeshRenderer>();
                    renderer.sharedMaterial = material;
                    combinedObjects.Add(combinedObject);
                }
                //remove old children
                DeleteAllInParent(s);
                //Add new ones
                foreach (GameObject combinedObject in combinedObjects) {
                    combinedObject.transform.parent = s.transform;
                }
                return s;
            }
        }

        // Combines all children meshes into a single mesh, optimize it, and assign it to the given parent obj, using one of the children's materials
        /*public GameObject CombineMesh(GameObject obj) {
            Mesh mainMesh = new Mesh();
            Material material = null;
            List<CombineInstance> combineInstances = new List<CombineInstance>();
            MeshFilter[] filters = obj.GetComponentsInChildren<MeshFilter>();

            foreach (MeshFilter filter in filters) {
                if (filter.transform == obj.transform) continue;
                
                if (material == null)
                    material = filter.gameObject.GetComponent<MeshRenderer>().sharedMaterial;
                
                CombineInstance combineInstance = new CombineInstance();
                combineInstance.subMeshIndex = 0;
                combineInstance.mesh = filter.sharedMesh;
                combineInstance.transform = filter.transform.localToWorldMatrix;
                combineInstances.Add(combineInstance);
            }
            
            DeleteAllInParent(obj);
            
            mainMesh.CombineMeshes(combineInstances.ToArray());
            mainMesh.Optimize();
            MeshFilter meshFilter = obj.GetComponent<MeshFilter>();
            if (meshFilter == null)
                meshFilter = obj.AddComponent<MeshFilter>();
            meshFilter.mesh = mainMesh;
            
            MeshRenderer meshRenderer = obj.GetComponent<MeshRenderer>();
            if (meshRenderer == null)
                meshRenderer = obj.AddComponent<MeshRenderer>();
            meshRenderer.material = material;

            ApplyCollider(obj, mainMesh);
            return obj;
        }*/
        
        public Mesh CombineMesh(Mesh[] meshes) {
            Mesh mainMesh = new Mesh();
            List<CombineInstance> combineInstances = new List<CombineInstance>();

            for (int i = 0; i < meshes.Length; i++) {
                CombineInstance combineInstance = new CombineInstance();
                combineInstance.subMeshIndex = 0;
                combineInstance.mesh = meshes[i];
                combineInstance.transform = Matrix4x4.identity;
                combineInstances.Add(combineInstance);
            }
            mainMesh.CombineMeshes(combineInstances.ToArray());
            mainMesh.Optimize();
            CalculateUVs(mainMesh);
            return mainMesh;
        }

        //Boolean operations 
        public GameObject Unite(GameObject s0, GameObject s1) {
            Mesh m = CSG.Union(s0, s1);
            GameObject composite = new GameObject();
            composite.transform.parent = currentParent.transform;
            composite.AddComponent<MeshFilter>().sharedMesh = m;
            composite.AddComponent<MeshRenderer>().sharedMaterial = s0.GetComponent<MeshRenderer>().sharedMaterial;
            composite.name = "Union";
            ApplyCollider(composite, m);
            ApplyLOD(composite);
            GameObject.DestroyImmediate(s0);
            GameObject.DestroyImmediate(s1);
            return MakeStatic(composite);
        }

        public GameObject Subtract(GameObject s0, GameObject s1) {
            Mesh m = CSG.Subtract(s0, s1);
            GameObject composite = new GameObject();
            composite.transform.parent = currentParent.transform;
            composite.AddComponent<MeshFilter>().sharedMesh = m;
            composite.AddComponent<MeshRenderer>().sharedMaterial = s0.GetComponent<MeshRenderer>().sharedMaterial;
            composite.name = "Subtraction";
            ApplyCollider(composite, m);
            ApplyLOD(composite);
            GameObject.DestroyImmediate(s0);
            GameObject.DestroyImmediate(s1);
            return MakeStatic(composite);
        }

        public GameObject Intersect(GameObject s0, GameObject s1) {
            Mesh m = CSG.Intersect(s0, s1);
            GameObject composite = new GameObject();
            composite.transform.parent = currentParent.transform;
            composite.AddComponent<MeshFilter>().sharedMesh = m;
            composite.AddComponent<MeshRenderer>().sharedMaterial = s0.GetComponent<MeshRenderer>().sharedMaterial;
            composite.name = "Intersection";
            ApplyCollider(composite, m);
            ApplyLOD(composite);
            GameObject.DestroyImmediate(s0);
            GameObject.DestroyImmediate(s1);
            return MakeStatic(composite);
        }

        public void SubtractFrom(GameObject s0, GameObject s1) {
            Mesh m = CSG.Subtract(s0, s1);
            s0.GetComponent<MeshFilter>().sharedMesh = m;
            ApplyCollider(s0, m);
            GameObject.DestroyImmediate(s1);
            s0.name += " Subtraction";
        }

        //Blocks
        //We could use Prefabs for this but they can only be used with the Unity Editor and I'm not convinced we want to depend on it.

        //Creating instances
        public GameObject CreateBlockInstance(GameObject block, Vector3 position, Vector3 vx, Vector3 vy, float scale) {
            GameObject obj = InstantiateFamily(block, position, vx, vy, scale);
            obj.SetActive(true);
            return MakeStatic(obj);
            //return obj;
        }

        //Creating blocks
        public GameObject CreateBlockFromFunc(String name, Func<List<GameObject>> f) =>
            CreateBlockFromShapes(name, f().ToArray());

        public GameObject CreateBlockFromShapes(String name, GameObject[] objs) {
            GameObject block = new GameObject(name);
            block.SetActive(false);
            foreach (GameObject child in objs) {
                child.transform.parent = block.transform;
            }
            return block;
        }


        //BIM

        public GameObject InstantiateFamily(GameObject family, Vector3 pos, Vector3 vx, Vector3 vy, float scale) {
            Quaternion rotation = Quaternion.LookRotation(vx, vy);
            GameObject s = GameObject.Instantiate(family);
            s.transform.parent = currentParent.transform;
            s.transform.localRotation = rotation * s.transform.localRotation;
            s.transform.localPosition = pos;
            s.transform.localScale *= scale;
            return MakeStatic(s);
        }

        public GameObject InstantiateBIMElement(GameObject family, Vector3 pos, float angle) {
            GameObject s = GameObject.Instantiate(family);
            s.transform.parent = currentParent.transform;
            s.transform.localRotation = Quaternion.Euler(0, Mathf.Rad2Deg * angle, 0) * s.transform.localRotation;
            s.transform.localPosition += pos;
            return MakeStatic(s);
        }

        // This helps improve performance BUT it prevents lights from being dynamically computed
        // This should only be done if we plan to bake
        public GameObject MakeStatic(GameObject s) {
            if (makeStatic) {
                s.isStatic = true;
                foreach (Transform trans in s.transform) {
                    MakeStatic(trans.gameObject);
                }
            }
            return s;
        }

        public GameObject Slab(Vector3[] contour, Vector3[][] holes, float h, Material material) =>
            ExtrudeContour(contour, holes, new Vector3(0, h, 0), material);

        public GameObject BeamRectSection(Vector3 position, Vector3 vx, Vector3 vy, float dx, float dy, float dz, float angle, Material material) =>
            RightCuboidNamed("Beam", position, vx, vy, dx, dy, dz, angle, material);

        public GameObject BeamCircSection(Vector3 bot, float radius, Vector3 top, Material material) =>
            CylinderNamed("Beam", bot, radius, top, material);

        public GameObject Panel(Vector3[] pts, Vector3 n, Material material) {
            Vector3[] bps = ReverseIfNeeded(pts, -n);
            Vector3[] tps = bps.Select(p => p + n).Reverse().ToArray();
            GameObject s = new GameObject("Panel");
            s.transform.parent = currentParent.transform;
            Mesh botMesh = CreatePolygonMesh(bps);
            Mesh topMesh = CreatePolygonMesh(tps);
            Array.Reverse(bps);
            Mesh sideMesh = CreateQuadMesh(bps, tps);
            MeshFilter meshFilter = s.AddComponent<MeshFilter>();
            meshFilter.sharedMesh = CombineMesh(new Mesh[] { botMesh, topMesh, sideMesh });
            s.AddComponent<MeshRenderer>();
            ApplyMaterial(s, material);
            ApplyCollider(s);
            ApplyLOD(s);
            return MakeStatic(s);
        }

        public void SetView(Vector3 position, Vector3 target, float lens) {
            playerTransform.position = position - mainCamera.gameObject.transform.localPosition;
            //mainCameraTransform.rotation = Quaternion.FromToRotation(mainCameraTransform.forward, target - position);
            mainCamera.transform.LookAt(target);
            mainCamera.focalLength = lens;
            Canvas.ForceUpdateCanvases();
        }
        public Vector3 ViewCamera() => mainCamera.transform.position;
        public Vector3 ViewTarget() => mainCamera.transform.position + mainCamera.transform.forward;
        public float ViewLens() => mainCamera.focalLength;

        public void SetResolution(int width, int height) {
            Screen.SetResolution(width, height, false);
        }

        public void ScreenShot(String path) {
            ScreenCapture.CaptureScreenshot(path);
        }

        public void createTerrain() {
            GameObject terrain = new GameObject("Terrain");
            terrain.transform.parent = currentParent.transform;
            TerrainData terrainData = new TerrainData();
            terrainData.size = new Vector3(10, 600, 10);
            terrainData.heightmapResolution = 512;
            terrainData.baseMapResolution = 1024;
            terrainData.SetDetailResolution(1024, 16);

            //int width = terrainData.heightmapWidth;
            //int height = terrainData.heightmapHeight;

            TerrainCollider collider = terrain.AddComponent<TerrainCollider>();
            Terrain terrain_aux = terrain.AddComponent<Terrain>();
            collider.terrainData = terrainData;
            terrain_aux.terrainData = terrainData;
            MakeStatic(terrain);
        }

        public void SelectGameObjects(GameObject[] objs) {
            //Deselect all
            foreach (KeyValuePair<GameObject, Color> entry in highlighted) {
                entry.Key.GetComponent<Renderer>().material.color = entry.Value;
            }
            highlighted.Clear();
            foreach (GameObject obj in objs) {
                Material mat = obj.GetComponent<Renderer>().material;
                highlighted.Add(obj, mat.color);
                mat.color = Color.green;
            }
        }

        public void MakeStaticGameObjects(bool val) {
            makeStatic = val;
        }

        public void StartSelectingGameObject() {
            selectedGameObject = null;
            inSelectionProcess = true;
        }

        public int IndexedSelfOrParent(GameObject obj, List<GameObject> objs) {
            int idx = objs.IndexOf(obj);
            if (idx >= 0) {
                return idx;
            } else {
                if (obj.transform == null) {
                    return -1;
                } else {
                    return IndexedSelfOrParent(obj.transform.parent.gameObject, objs);
                }
            }
        }

        public int SelectedGameObjectId(bool existing) {
            if (selectedGameObject != null) {
                List<GameObject> shapes = channel.shapes;
                if (existing) {
                    int idx = IndexedSelfOrParent(selectedGameObject, shapes);
                    if (idx >= 0) {
                        inSelectionProcess = false;
                        return idx;
                    } else {
                        return -2;
                    }
                } else {
                    inSelectionProcess = false;
                    shapes.Add(selectedGameObject);
                    return shapes.Count - 1;
                }
            } else {
                return -2;
            }
        }

        public void SwitchToParent(GameObject newParent) {
            SetActive(currentParent, false);
            SetActive(newParent, true);
            currentParent = newParent;
        }

        public void OptimizeParent() {
            if (enableMergeParent)
                MergeParent();
            StaticBatchingUtility.Combine(currentParent.gameObject);
        }
        
        public GameObject ApplyLOD(GameObject g) {
            if (!applyLOD)
                return g;
            Mesh mesh = g.GetComponent<MeshFilter>()?.sharedMesh;
            if (mesh.vertexCount < 30) // HACK! So it doesn't simplify already simple meshes
                return g;

            LODGenerator.GenerateLODGroup(g, lodLevels, true, lodSimplificationOptions);
            return g;
        }

        public void MergeParent() {
            if (currentParent.GetComponent<MeshFilter>() != null || currentParent.transform.childCount == 0) return; // Return if the current parent has already merged its mesh
            Renderer[] renderers = LODGenerator.GetChildRenderersForLOD(currentParent);
            var meshRenderers = (from renderer in renderers
                where renderer.enabled && renderer as MeshRenderer != null 
                select renderer as MeshRenderer).ToArray();

            Material[] materials;
            Mesh mesh = MeshCombiner.CombineMeshes(currentParent.transform, meshRenderers, out materials);
            mesh.Optimize();
            MeshRenderer meshRenderer = currentParent.AddComponent<MeshRenderer>();
            meshRenderer.materials = materials;
            MeshFilter meshFilter = currentParent.AddComponent<MeshFilter>();
            meshFilter.sharedMesh = mesh;
            
            foreach (var renderer in renderers) {
                renderer.enabled = false;
            }
        }
        
        public void CalculateUVs(Mesh mesh) {
            if (!makeUVs) return;
            
            float scaleFactor = 0.5f;
            mesh.uv = UvCalculator.CalculateUVs(mesh.vertices, scaleFactor);
            Unwrapping.GenerateSecondaryUVSet(mesh);
        }
        
        public class UvCalculator {
         private enum Facing { Up, Forward, Right };
         
         public static Vector2[] CalculateUVs(Vector3[] v/*vertices*/, float scale)
         {
             var uvs = new Vector2[v.Length];
             Debug.Log(v.Length);
             
             for (int i = 0 ; i < uvs.Length; i += 3)
             {
                 int i0 = i;
                 int i1 = i+1;
                 int i2 = i+2;
                 
                 //Special handling if vertex count isn't a multiple of 3
                 if(i == uvs.Length - 1) {
                     i1 = 0;
                     i2 = 1;
                 }
                 if(i == uvs.Length - 2) {
                     i2 = 0;
                 }
                 
                 Vector3 v0 = v[i0];
                 Vector3 v1 = v[i1];
                 Vector3 v2 = v[i2];
                 
                 Vector3 side1 = v1 - v0;
                 Vector3 side2 = v2 - v0;
                 var direction = Vector3.Cross(side1, side2);
                 var facing = FacingDirection(direction);
                 switch (facing)
                 {
                 case Facing.Forward:
                     uvs[i0] = ScaledUV(v0.x, v0.y, scale);
                     uvs[i1] = ScaledUV(v1.x, v1.y, scale);
                     uvs[i2] = ScaledUV(v2.x, v2.y, scale);
                     break;
                 case Facing.Up:
                     uvs[i0] = ScaledUV(v0.x, v0.z, scale);
                     uvs[i1] = ScaledUV(v1.x, v1.z, scale);
                     uvs[i2] = ScaledUV(v2.x, v2.z, scale);
                     break;
                 case Facing.Right:
                     uvs[i0] = ScaledUV(v0.z, v0.y, scale);
                     uvs[i1] = ScaledUV(v1.z, v1.y, scale);
                     uvs[i2] = ScaledUV(v2.z, v2.y, scale);
                     break;
                 }
             }
             return uvs;
         }
         
         private static bool FacesThisWay(Vector3 v, Vector3 dir, Facing p, ref float maxDot, ref Facing ret)
         {
             float t = Vector3.Dot(v, dir);
             if (t > maxDot)
             {
                 ret = p;
                 maxDot = t;
                 return true;
             }
             return false;
         }
         
         private static Facing FacingDirection(Vector3 v)
         {
             var ret = Facing.Up;
             float maxDot = 0;
             
             if (!FacesThisWay(v, Vector3.right, Facing.Right, ref maxDot, ref ret))
                 FacesThisWay(v, Vector3.left, Facing.Right, ref maxDot, ref ret);
             
             if (!FacesThisWay(v, Vector3.forward, Facing.Forward, ref maxDot, ref ret))
                 FacesThisWay(v, Vector3.back, Facing.Forward, ref maxDot, ref ret);
             
             if (!FacesThisWay(v, Vector3.up, Facing.Up, ref maxDot, ref ret))
                 FacesThisWay(v, Vector3.down, Facing.Up, ref maxDot, ref ret);
             
             return ret;
         }
         
         private static Vector2 ScaledUV(float uv1, float uv2, float scale)
         {
             return new Vector2(uv1 / scale, uv2 / scale);
         }
     }
        
    }
}