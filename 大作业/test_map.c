#define _CRT_SECURE_NO_WARNINGS
#include<bits/stdc++.h>  


/*
使用方式：
开局：调用init_map
按下鼠标：调用click或right_click，之后使用 result_map 渲染界面，final_result 判断结束状态
*/


// 位置-状态表，用于渲染
#define no_operate 1//未操作
#define click_down 2//鼠标按下
#define no_mine 3//无雷
#define flag 4//插旗
#define mine_not_explode 5//有雷未爆（结算时用）
#define mine_explode 6//有雷已爆（结算时用）
#define minf_and_flag 7//有雷并插旗（结算时用）
#define num_1 11//数字1
#define num_2 12//数字2
#define num_3 13//数字3
#define num_4 14//数字4
#define num_5 15//数字5
#define num_6 16//数字6
#define num_7 17//数字7
#define num_8 18//数字8

//状态表，用于计算
#define not_mine 0//不是雷
#define is_mine 1//是雷


//地图

#define max_map_len 200//最大地图大小

//边界量，实际可访问的地图区域
int left_bound = 0;
int right_bound = 0;
int up_bound = 0;
int down_bound = 0;

//申请的地图
int mine_map[max_map_len][max_map_len] = { 0 };//地雷图
int operate_map[max_map_len][max_map_len] = { 0 };//操作图
int result_map[max_map_len][max_map_len] = { 0 };//渲染图，注意边界限制在0-right_bound；0-down_bound之间

//计数
int mine_count = 0;//地雷总数

//结算
int final_result = 0;

#define playing 0;
#define success 1;
#define falied 2;

//以下是公共的
void init_map( int mine_count ,int length ,int width );
void click( int i, int j );
void right_click( int i, int j );//插旗

//以下是私有的
void update_map();//计算地图，并更新结果





int main() {

    return 0;
}