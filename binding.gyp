{
    "targets": [
        {
            "target_name": "touch",
            "sources": ["touch.cpp"],
            "include_dirs": ["<!(node -e \"require('nan')\")"],
        }
    ]
}
