{
    "Source": "DarkArisenTerrain_Depth.azsl",
    "DepthStencilState": {
        "Depth": {
            "Enable": true,
            "CompareFunc": "LessEqual"
        }
    },
    "RasterState": {
        "CullMode": "Back",
        "depthBias": "10",
        "depthBiasSlopeScale": "4"
    },
    "ProgramSettings": {
        "EntryPoints": [
            {
                "name": "DepthPassVS",
                "type": "Vertex"
            }
        ]
    },
    "DrawList": "shadow"
}
