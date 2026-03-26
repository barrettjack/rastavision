// some notes re: .obj file parsing
// this .obj file loader will support, initially, a minimal subset of features
// supported by the file format. Specifically, it will parse lines of the form:
//
// o [object name]
// v [geometric vertex]
// vn [vertex normal]
// vt [texture coordinates]
// f [polygonal face element]
//
// Subject to the following limitations:
//
// v: vertices have no w coordinate and provide no colour
// see: https://en.wikipedia.org/wiki/Wavefront_.obj_file#Geometric_vertex
//
// vt: only u and v coordinates are provided (i.e. no w coordinate)
//
// f: face elements are triangles only
//
// QUESTIONS:
//
// why are there more vt lines than v, vn?
// and why are there more f lines than v, vt, vn?
// how do faces work?
// how should I represent and store this data in my program?
// what data is needed for the rendering pipeline and what are the expectations
// or norms regarding how it will be stored/represented?
