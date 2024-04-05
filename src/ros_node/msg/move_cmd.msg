std_msgs/Header header
int32 level      # 等级 0-导航 1-界面遥控 2-手柄/命令
float32 velocity # 速度 m/s
float32 vx       # 速度vx m/s
float32 vy       # 速度vy m/s
float32 omega    # 角速度 rad/s
float32 angle    # 角度 rad
int8 direction   # 方向 0-无 1-前向 2-反向