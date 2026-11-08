{
    "Source": "DarkArisenFoliage_Depth.azsl",

    "DepthStencilState": {
        "Depth": {
            "Enable": true,
            "CompareFunc": "GreaterEqual"
        }
    },

    "RasterState": {
        "CullMode": "None"
    },

    "ProgramSettings": {
        "EntryPoints": [
            {
                "name": "DepthPassVS",
                "type": "Vertex"
            },
            {
                "name": "DepthPassPS",
                "type": "Fragment"
            }
        ]
    },

    "DrawList": "depth"
}
