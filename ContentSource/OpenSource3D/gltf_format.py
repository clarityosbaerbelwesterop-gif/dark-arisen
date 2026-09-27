"""GLB / embedded glTF interchange without external dependencies."""

import base64
import json
import struct
from pathlib import Path


def unpack_glb(raw: bytes):
    assert raw[:4] == b'glTF' and struct.unpack_from('<I',raw,4)[0] == 2
    assert len(raw) == struct.unpack_from('<I',raw,8)[0]
    jlen,kind=struct.unpack_from('<I4s',raw,12)
    assert kind == b'JSON'
    doc=json.loads(raw[20:20+jlen])
    off=20+jlen
    blen,kind=struct.unpack_from('<I4s',raw,off)
    assert kind == b'BIN\0'
    return doc, bytes(raw[off+8:off+8+blen])


def embedded_gltf(doc: dict, binary: bytes) -> bytes:
    doc=dict(doc)
    doc['buffers']=[dict(doc['buffers'][0],uri='data:application/octet-stream;base64,'+base64.b64encode(binary).decode('ascii'))]
    return (json.dumps(doc,separators=(',',':'),ensure_ascii=False)+'\n').encode('utf-8')


def read_embedded_gltf(path: Path):
    doc=json.loads(path.read_text(encoding='utf-8'))
    uri=doc['buffers'][0].pop('uri')
    assert uri.startswith('data:application/octet-stream;base64,')
    binary=base64.b64decode(uri.partition(',')[2],validate=True)
    assert len(binary)>=doc['buffers'][0]['byteLength']
    return doc,binary
