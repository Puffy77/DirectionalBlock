# Directional Blocks
Brings the Directional Blocks from Super Mario 3D Land into SMG2!
If path ID is set, hitting the block will instead move it along the path.

## Obj_args
| Arg | Type | Name | Description | Default |
| --- | --- | --- | --- | --- | 
| 0 | Float | Step Size | How far the block travels when activated. | 100.0 | 
| 1 | Int | Max Steps | How many steps can the block take before it stops. | 5 |
| 2 | Int | Falling Type | Does the block fall or not, see below for more info. | 0 |
| 3 | Int | Fall Delay | The time, in frames, it takes for the block to start falling. | 120 |
| 4 | Float | Fall Speed | The speed at which the block falls. | 15.0 |
| 5 | Int | Switch B Behavior | Activate SW_B on hit (0) or at max steps (1). | 0 |
| 6 | Int | Color | Changes the block's color, see below for more info. | 7 |

## Falling Types
| Value | Description |
| --- | --- |
| 0 | Does not fall |
| 1 | Only falls at max steps |
| 2 | Can fall anytime |

## Colors
| Value | Color |
| --- | --- |
| 0 | Red |
| 1 | Orange |
| 2 | Yellow |
| 3 | Light Green |
| 4 | Dark Green |
| 5 | Light Blue |
| 6 | Dark Blue |
| 7 | Purple |
| 8 | Pink |
| 9 | White |
| 10 | Black |
| 11 | Gray* |

*Not reccomended, as its used to show when the block has reached the maximum number of steps.

## Switches
| Switch | Type | Description |
| --- | --- | --- |
| SW_A | Read | When active, activates the block |
| SW_B | Write | Activates the switch based on the specified Obj_Arg 5 |

## ActionSound
This module uses custom sounds. You must add them to ActionSound.bcsv
```csv
DirectionalBlock,OjBlockMove,SE_OJ_PUNCHING_BLOCK_PUNCHED,,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0,0,-1,-1,0,-1,-1
DirectionalBlock,OjBlockStopped,SE_OJ_POW_STAR_MOVE_END,,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0,0,-1,-1,0,-1,-1
DirectionalBlock,OjBlockFall,SE_OJ_FALL_BLOCK_FALL,,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0,0,-1,-1,0,-1,-1
DirectionalBlock,OjBlockCrash,SE_OJ_FALL_BLOCK_LAND,,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0,0,-1,-1,0,-1,-1
```
