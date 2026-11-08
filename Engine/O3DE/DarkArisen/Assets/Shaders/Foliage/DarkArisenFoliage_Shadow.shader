{
    "Source": "DarkArisenFoliage_Depth.azsl",

    "DepthStencilState": {
        "Depth": {
            "Enable": true,
            "CompareFunc": "LessEqual"
        }
    },

    "RasterState": {
        "CullMode": "None",
        "depthBias": "10",
        "depthBiasSlopeScale": "4"
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

    "DrawList": "shadow"
}
