{
    "Source": "DarkArisenTerrain_Depth.azsl",

    "DepthStencilState": {
        "Depth": {
            "Enable": true,
            "CompareFunc": "GreaterEqual"
        }
    },

    "RasterState": {
        "CullMode": "Back"
    },

    "ProgramSettings": {
        "EntryPoints": [
            {
                "name": "DepthPassVS",
                "type": "Vertex"
            }
        ]
    },

    "DrawList": "depth"
}
