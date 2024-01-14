#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
使用方式：
开局：调用init_map
按下鼠标：调用click或right_click，之后使用 result_map 渲染界面，final_result 判断结束状态
*/


// 位置-状态表，用于渲染
#define no_operate 0//未操作
#define click_down 1//鼠标按下
#define no_mine 2//无雷
#define flag 3//插旗
#define mine_not_explode 4//有雷未爆（结算时用）
#define mine_explode 5//有雷已爆（结算时用）
#define mine_and_flag 6//有雷并插旗（结算时用）
#define num_0 10//数字0，用于base
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
int mine_count = 0;//地雷总数，同时是现有棋子数

//结算
int final_result = 0;

#define playing 0
#define success 1
#define failed 2

//以下是公共的，返回0代表操作成功，返回1代表操作失败
int init_map( int mine_count, int length, int width, unsigned int seed);//初始化地图，传入地雷数量，地图长，地图宽，随机种子
int click( int i, int j );
int right_click( int i, int j );//插旗

//以下是私有的
void update_map( int i, int j );//计算地图
int check_point_in_map( int i, int j );//计算点是否在坐标轴内
int check_mine_around( int i, int j );//检查[i][j]周围雷的个数并返回
int check_is_mine( int i, int j );  //检查是否是雷
void check_for_falied();//检查失败情况
void check_for_success();//检查成功情况
int check_point_in_map( int i, int j );//检查点是否在图内，1代表在，0代表不在
void pr( int** a );

int init_map( int mine_count_all, int length, int width , unsigned int seed ) {

    //检查输入合法性
    
    if ( length > max_map_len ) {
        return 1;
    }
    if ( width > max_map_len ) {
        return 1;
    }
    if ( mine_count_all >= length * width ) {
        return 1;
    }
    //设置种子
    if ( seed == 0 ) {
        srand( (unsigned)time( NULL ) );
    }
    else {
        srand( seed );
    }
    //初始化地图
    left_bound = 0;
    right_bound = length;
    up_bound = 0;
    down_bound = width;

    memset( mine_map, 0, sizeof( mine_map ) );
    memset( operate_map, 0, sizeof( operate_map ) );
    memset( result_map, 0, sizeof( result_map ) );

    final_result = playing;
    //随机布雷
    int rand_x = 0;
    int rand_y = 0;
    int mine_count_now = 0;
    mine_count = mine_count_all;
    while ( mine_count_now < mine_count_all ) {
        rand_x = rand() % width;
        rand_y = rand() % length;
        if ( mine_map[rand_x][rand_y] != is_mine ) {
            mine_map[rand_x][rand_y] = is_mine;
            mine_count_now++;
        }
    }

    return 0;

}

int click( int i, int j ) {
    if ( operate_map[i][j] == no_operate ) {
        update_map( i, j );
    }
    //更新完成
    check_for_success();
    return 0;
}

int right_click( int i, int j ) {
    //如果是旗，取消
    
    if ( operate_map[i][j] == flag ) {
        mine_count++;
        operate_map[i][j] = no_operate;
        return 0;
    }
    //如果为空，放置为旗，并检查是否过关
    else if ( operate_map[i][j] == no_operate ) {
        if ( mine_count == 0 ) {
            return 1;
        }
        operate_map[i][j] = flag;
        mine_count--;
        check_for_success();
    }
    //不允许其他操作
    return 1;
}

void update_map( int i, int j ) {
    //点是否在图内
    if ( !check_point_in_map( i, j ) ) {
        return;
    }
    //点是否可更新
    if ( !(operate_map[i][j] == no_operate) ) {
        return;
    }
    //更新点的状态
    if ( mine_map[i][j] == is_mine ) {
        //触雷
        operate_map[i][j] = mine_explode;
        result_map[i][j] = mine_explode;
        check_for_falied();

    }
    else {
        int mine_around = check_mine_around( i, j );
        if ( mine_around == 0 ) {
            operate_map[i][j] = no_mine;
            result_map[i][j] = no_mine;
            //周围没有雷，可以递归
            update_map( i - 1, j );
            update_map( i + 1, j );
            update_map( i, j - 1 );
            update_map( i, j + 1 );

        }
        else {
            //有雷，递归可能爆炸
            operate_map[i][j] = num_0 + mine_around;
            result_map[i][j] = num_0 + mine_around;
        }
    }
}

int check_mine_around( int i, int j ) {
    //检查[i][j]周围雷的个数并返回
    int sum = 0;
    int c = 0;

    c = check_is_mine( i - 1, j - 1 );
    sum += c;
    c = check_is_mine( i - 1, j  );
    sum += c;
    c = check_is_mine( i - 1, j + 1 );
    sum += c;

    c = check_is_mine( i, j - 1 );
    sum += c;
    c = check_is_mine( i, j + 1 );
    sum += c;

    c = check_is_mine( i +1, j - 1 );
    sum += c;
    c = check_is_mine( i + 1, j  );
    sum += c;
    c = check_is_mine( i + 1, j + 1 );
    sum += c;
    
    return sum;

}

int check_is_mine( int i, int j ) {
    //检查是否是雷
    return check_point_in_map( i, j ) && mine_map[i][j] == is_mine;
}

void check_for_falied() {
    int i = 0;
    int j = 0;

    for ( i = left_bound; i < right_bound; i++ ) {
        for ( j = up_bound; j < down_bound; j++ ) {
            if ( mine_map[i][j] == is_mine ) {
                //调用这个函数时，已经处理了有雷爆炸的情况
                //因此只需要更新旗子相关
                if ( operate_map[i][j] == flag ) {
                    result_map[i][j] = mine_and_flag;
                }
                else if( operate_map[i][j] == no_operate ) {
                    result_map[i][j] = mine_not_explode;
                }
            }
        }
    }
    final_result = failed;

}

void check_for_success() {
    int i = 0;
    int j = 0;
    if ( mine_count != 0 ) {
        return;//还有雷未排完
    }
    for ( i = left_bound; i < right_bound; i++ ) {
        for ( j = up_bound; j < down_bound; j++ ) {
            //允许的状态只有：无雷，数字和已插旗
            if(
                !(
                    operate_map[i][j] == no_mine ||
                    operate_map[i][j] == flag ||
                    num_1 <= operate_map[i][j] && operate_map[i][j] <= num_8
                    )
                )

                return;//检查未通过
        }
    }

    //检查通过，刷新地图
    for ( i = left_bound; i < right_bound; i++ ) {
        for ( j = up_bound; j < down_bound; j++ ) {
            //插旗
            if (
                operate_map[i][j] == flag && mine_map[i][j] == is_mine
                ) {
                result_map[i][j] = mine_and_flag;
            }
            else if (
                operate_map[i][j] == no_mine
                ) {
                result_map[i][j] = no_mine;
            }
            else {
                // num_1 <= operate_map[i][j] && operate_map[i][j] <= num_8
                result_map[i][j] = operate_map[i][j];
            }
        }
    }

    final_result = success;
}

int check_point_in_map( int i, int j ) {
    //检查点是否在图内，1代表在，0代表不在
    int len = left_bound <= i && i <= right_bound;
    int wid = up_bound <= j && j <= down_bound;
    return len & wid;
}

void pr( int a[max_map_len][max_map_len]) {
    for ( int i = left_bound; i < right_bound; i++ ) {
        for ( int j = up_bound; j < down_bound; j++ ) {
            printf( "%d\t", a[i][j] );
        }
        printf( "\n" );
    }
    printf( "\n" );
}


int main() {
    int mine = 0, l = 0, w = 0;
    while ( 1 ) {
        scanf( "%d %d %d", &mine, &l, &w );
        init_map( mine, l, w, 1 );
        pr( mine_map );
        int a = 0, b = 0, c = 0;
        int op_res = 0;
        while ( 1 ) {
            if ( final_result != playing ) {
                break;
            }
            scanf( "%d %d %d", &a, &b, &c );
            if ( c == 0 ) {
                op_res = click( a, b );
            }
            else {
                op_res = right_click( a, b );
            }
            printf( "%d\n", op_res );
            printf( "%s\n", "mine_map" );
            pr( mine_map );
            printf( "%s\n", "operate_map" );
            pr( operate_map );
            printf( "%s\n", "result_map" );
            pr( result_map );
        }
        printf( "%d", final_result );
    }
    return 0;
}

