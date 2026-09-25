{
    "Source": "DarkArisenOcean_Depth.azsl",

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
            }
        ]
    },

    "DrawList": "depth"
}
