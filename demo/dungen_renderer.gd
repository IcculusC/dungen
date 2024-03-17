extends Dungen

@onready var tile_map := $TileMap
@onready var camera := $Camera2D

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
  generate()

  var bounding_rect := Rect2i()
  var map = get_all()
  
  var all_rects = []
  
  for p in map.paths:
    all_rects.push_back(p)
    bounding_rect = bounding_rect.expand(p.get_center())
  for r in map.rooms:
    bounding_rect = bounding_rect.expand(r.get_center())
    all_rects.push_back(r)
    
  bounding_rect = bounding_rect.grow(min(bounding_rect.size.x, bounding_rect.size.y) / 2)
  
  camera.set_position(tile_map.map_to_local(bounding_rect.get_center()))
  
  for r in all_rects:
    draw_tilemap_rectangle(r)

  

func draw_tilemap_rectangle(rect: Rect2i) -> void:
  for x in range(rect.position.x, rect.position.x + rect.size.x):
    for y in range(rect.position.y, rect.position.y + rect.size.y):
      tile_map.set_cell(0, Vector2i(x, y), 0, Vector2i(0, 0))
