//------------------------------------------------------------------------------
/// @file
/// @brief    HPCAnswer.hpp の実装 (解答記述用ファイル)
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2015 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください

//------------------------------------------------------------------------------
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <iomanip>      // std::setfill, std::setw
#include <random>       // std::default_random_engine
//#include <chrono>       // std::chrono::system_clock
//https://ja.wikipedia.org/wiki/Composite_%E3%83%91%E3%82%BF%E3%83%BC%E3%83%B3
#include "HPCAnswer.hpp"
#include "HPCMath.hpp"
namespace {
    using namespace std;
    using namespace hpc;
    constexpr int PINF = std::numeric_limits<int>::max();
    constexpr int MINF = std::numeric_limits<int>::lowest();
    template<class T = int> inline bool within(T min_x, T x, T max_x){ return min_x<=x&&x<max_x; }
    class BFSQuery{ public: int x,y,d; };
    constexpr int dxy[] = {-1,0,1,0,0,-1,0,1};

    class MindSet{
    public:
        //virtual think(){}
    };
    class Brain{
    public:
        Brain(){};
        inline void init(const Stage& aStage){
            items_ = aStage.items();
            field_ = aStage.field();

            num_of_items_ = items_.count();
            width_ = field_.width();
            height_ = field_.height();
            home_ = field_.officePos();
            build_dmap();
            build_dtable();
        };
        inline void think(const Stage& aStage, vector<vector<int>>* items_p, vector<vector<int>>* actions_p){
            //vector<vector<int>>& items = *items_p;
            //vector<vector<int>>& actions = *actions_p;
            think(aStage,*items_p,*actions_p);
        }
        inline void think(const Stage& aStage, vector<vector<int>>& items, vector<vector<int>>& actions){
            init(aStage);
            think_sequenses(items);
            build_actions(items,actions);
        }
    protected:
        vector<vector<vector<int>>> dmap_to_items_;
        vector<vector<int>> dmap_to_home_;
        vector<vector<int>> dtable_;
        vector<int> dtable_home_;
        int width_,height_;
        Pos home_;
        ItemCollection items_;
        int num_of_items_;
        Field field_;
        inline void build_dmap(){
            init_dmap();
            calc_dmap();
        };
        inline void init_dmap(){
            dmap_to_items_ = 
                vector<vector<vector<int>>>(num_of_items_,
                    vector<vector<int>>(width_,
                        vector<int>(height_,-1)));
            dmap_to_home_ = vector<vector<int>>(width_,
                        vector<int>(height_,-1));
        };
        inline void calc_dmap(){
            for(int i = 0; i < num_of_items_; ++i){
                Pos dest = items_[i].destination();
                //int x_zero = dest.x, y_zero = dest.y;
                //bfs_dmap(&dmap_to_items_[i],dest.x,dest.y);
                bfs_dmap(dmap_to_items_[i],dest.x,dest.y);
            }
            //bfs_dmap(&dmap_to_home_,home_.x,home_.y);
            bfs_dmap(dmap_to_home_,home_.x,home_.y);
        };
        inline void bfs_dmap(vector<vector<int>>* dmap_p,int x_zero,int y_zero){
            //vector<vector<int>& field = *dmap_p;
            bfs_dmap(*dmap_p,x_zero,y_zero);
        }
        inline void bfs_dmap(vector<vector<int>>& dmap,int x_zero,int y_zero){
            dmap[x_zero][y_zero] = 0;
            queue<BFSQuery> ques;
            ques.push({x_zero,y_zero,0});
            while(!ques.empty()){
                auto que = ques.front(); ques.pop();
                int x = que.x, y = que.y, d = que.d;
                for(int i = 0; i < 4; ++i){
                    int nx = x+dxy[i*2], ny = y+dxy[i*2+1];
                    if(within(0,nx,width_)&&within(0,ny,height_)){
                        if((!field_.isWall(nx,ny))&&dmap[nx][ny]==-1){
                            dmap[nx][ny]=d+1;
                            ques.push({nx,ny,d+1});
                        }
                    }
                }
            }
            return ;
            for(int i = 0; i < width_; ++i){
                for(int j = 0; j < height_; ++j){
                    cout << " " << setfill(' ') << setw(3) << right << dmap[i][j];
                    //cout << " " << dmap[i][j];
                }
                cout << endl;
            }
        }
        inline void build_dtable(){
            init_dtable();
            calc_dtable();
        }
        inline void init_dtable(){
            dtable_ = vector<vector<int>>(num_of_items_,
                vector<int>(num_of_items_,-1));
            dtable_home_ = vector<int>(num_of_items_,-1);
        };
        inline void calc_dtable(){
            int home_x = home_.x,home_y = home_.y;
            for(int i = 0; i < num_of_items_; ++i){
                const Pos& dest = items_[i].destination();
                int dest_x = dest.x,dest_y = dest.y;
                dtable_home_[i] = dmap_to_items_[i][home_x][home_y];
                for(int j = 0; j < num_of_items_; ++j){
                    int dist = dmap_to_items_[j][dest_x][dest_y];
                    dtable_[i][j] = dist;
                    //dtable_[j][i] = dist;
                }
            }
        };
        inline void think_sequenses(vector<vector<int>>& items){
            //cout << "!think_sequenses" << endl;
            items = vector<vector<int>>(4,vector<int>(0));
            vector<int> perm(num_of_items_,0);
            iota(perm.begin(), perm.end(), 0);
            constexpr unsigned seed = 114514;
            shuffle(perm.begin(), perm.end(), default_random_engine(seed));

            vector<int> weights(4,0);

            for(int i = 0; i < num_of_items_; ++i){
                int id = perm[i];
                int period = items_[id].period();
                if(period!=-1){
                    items[period].push_back(id);
                    weights[period]+=items_[id].weight();
                    perm[i]=-1;
                }
            }

            int p=0;
            for(int i : perm){
                if(i!=-1){
                    int w = items_[i].weight();
                    while(true){
                        if(Parameter::TruckWeightCapacity<weights[p]+w){
                            p = (p+1)%4;
                            continue;
                        }else{
                            weights[p]+=w;
                            items[p].push_back(i);
                            p = (p+1)%4;
                            break;
                        }
                    }
                }
            }
            for(auto& seq : items){
                int loop_max = calc_loop_max(seq.size());
                int best_score = PINF;
                vector<int> best_seq;
                for(int i = 0; i < loop_max+1; ++i){
                    next_permutation(seq.begin(),seq.end());
                    int score = calc_score(seq);
                    if(score<best_score){
                        best_score = score;
                        best_seq = seq;
                    }
                }
                seq = best_seq;
            }
        };

        inline int calc_loop_max(int size){
            int ret = 1;
            constexpr int LOOP_MAX_MAX = 1000;
            for(int i = 1; i < size; ++i){
                ret*=(i+1);
                if(LOOP_MAX_MAX<ret){
                    ret = LOOP_MAX_MAX;
                    break;
                }
            }
            //cout << size << " " << ret << endl;
            return ret;
        }
        inline int calc_score(const vector<int>& seq){
            int ret=0,last=-1;
            int weight = 3;
            for(auto i : seq){
                weight+=items_[i].weight();
            }
            for(auto next : seq){
                int dist;
                if(last==-1){
                    dist = dtable_home_[next];
                }else{
                    dist = dtable_[next][last];
                }
                last=next;
                ret+=dist*weight;
                weight-=items_[next].weight();
            }


            ret += dtable_home_[last]*weight;
            return ret;
        }
        inline void build_actions(const vector<vector<int>>& items,vector<vector<int>>& actions){
            actions = vector<vector<int>>(4,vector<int>(0));
            for(int period = 0; period < 4; ++period){
                Pos pos = home_;
                //cout << "period :" << period << ", size :" << items[period].size() << endl;
                auto& targets = items[period];
                for(auto target : targets){
                    add_sequense(dmap_to_items_[target],pos,actions[period]);
                }
                add_sequense(dmap_to_home_,pos,actions[period]);
            }
        }
        inline void add_sequense(const vector<vector<int>>& dmap_to_dest,Pos& pos,vector<int>& sequense){
            int dist = dmap_to_dest[pos.x][pos.y];
            while(dist!=0){
                for(int i = 0; i < 4; ++i){
                    int nx = pos.x+dxy[i*2], ny = pos.y+dxy[i*2+1];
                    if(within(0,nx,width_)&&within(0,ny,height_)){
                        if((dist-1)==dmap_to_dest[nx][ny]){
                            dist = dmap_to_dest[nx][ny];
                            sequense.push_back(i);
                            pos=pos.move(Action(i));
                            break;
                        }
                    }
                }
            }
        }
    };
}

/// プロコン問題環境を表します。
namespace hpc {

    //------------------------------------------------------------------------------
    /// 各ステージ開始時に呼び出されます。
    ///
    /// ここで、各ステージに対して初期処理を行うことができます。
    ///
    /// @param[in] aStage 現在のステージ。
    Brain smartest_brain;
    vector<vector<int>> items;
    vector<vector<int>> actions;
    int period,turn,stage=-1;
    constexpr bool ISNOT_UNKO = true;
    void Answer::Init(const Stage& aStage){
        ++stage;
        //cout << "stage " << stage << endl;
        smartest_brain.think(aStage,items,actions);
        period = -1;
        turn   = -1;
    }

    void InitPeriod_old(const Stage& aStage, ItemGroup& aItemGroup);
    Action GetNextAction_old(const Stage& aStage);
    //------------------------------------------------------------------------------
    /// 各配達時間帯開始時に呼び出されます。
    ///
    /// ここで、この時間帯に配達する荷物をトラックに積み込みます。
    /// どの荷物をトラックに積み込むかを決めて、引数の aItemGroup に対して
    /// setItem で荷物番号を指定して積み込んでください。
    ///
    /// @param[in] aStage 現在のステージ。
    /// @param[in] aItemGroup 荷物グループ。
    void Answer::InitPeriod(const Stage& aStage, ItemGroup& aItemGroup){

        ++period;
        turn=-1;
        //cout << "period " << period << endl;
        if(ISNOT_UNKO){
            for(auto i : items[period]){ aItemGroup.addItem(i); }
            return ;
        }
        InitPeriod_old(aStage,aItemGroup);
    }

    //------------------------------------------------------------------------------
    /// 各ターンでの動作を返します。
    ///
    /// @param[in] aStage 現在ステージの情報。
    ///
    /// @return これから行う動作を表す Action クラス。
    Action Answer::GetNextAction(const Stage& aStage){

        turn++;
        //cout << "turn " << turn << endl;
        if(ISNOT_UNKO){
            //cout << actions[period][turn] << endl;
            return Action(actions[period][turn]);
        }
        return GetNextAction_old(aStage);
    }

    //------------------------------------------------------------------------------
    /// 各配達時間帯終了時に呼び出されます。
    ///
    /// ここで、この時間帯の終了処理を行うことができます。
    ///
    /// @param[in] aStage 現在のステージ。
    /// @param[in] aStageState 結果。Playingならこの時間帯の配達完了で、それ以外なら、何らかのエラーが発生した。
    /// @param[in] aCost この時間帯に消費した燃料。エラーなら0。
    void Answer::FinalizePeriod(const Stage& aStage, StageState aStageState, int aCost)
    {
        if (aStageState == StageState_Failed) {
            // 失敗したかどうかは、ここで検知できます。
        }
    }

    //------------------------------------------------------------------------------
    /// 各ステージ終了時に呼び出されます。
    ///
    /// ここで、各ステージに対して終了処理を行うことができます。
    ///
    /// @param[in] aStage 現在のステージ。
    /// @param[in] aStageState 結果。Completeなら配達完了で、それ以外なら、何らかのエラーが発生した。
    /// @param[in] aScore このステージで獲得したスコア。エラーなら0。
    void Answer::Finalize(const Stage& aStage, StageState aStageState, int aScore)
    {
        if (aStageState == StageState_Failed) {
            // 失敗したかどうかは、ここで検知できます。
        }
        else if (aStageState == StageState_TurnLimit) {
            // ターン数オーバーしたかどうかは、ここで検知できます。
        }
    }

    void InitPeriod_old(const Stage& aStage, ItemGroup& aItemGroup){
        if (aStage.period() == 0) {
            return;
        }
        for (int i = 0; i < aStage.items().count(); ++i) {
            // まだ配達されてない荷物かどうか調べる
            if (aStage.getTransportState(i) == TransportState_NotTransported) {
                // 配達されてない荷物なので積み込む
                aItemGroup.addItem(i);
            }
        }
    }

    Action GetNextAction_old(const Stage& aStage){
        static Random random; // デフォルトのシード値を使う
        static Pos prev; // 初期値は重要ではない。(前のゲームの値が残っていても気にしない)

        for (int retry = 0; ; ++retry) {
            Action a = static_cast<Action>(random.randTerm(4));
            Pos nextPos = aStage.truck().pos().move(a);
            if (aStage.field().isWall(nextPos) == false) { // 動けるか
                if (retry < 50 && nextPos == prev) {
                    // 前にいた場所を避ける。
                    // これで、同じような場所をウロウロしてなかなか進まないのを防げる。
                    // ただし、50回やっても見つからないときは、諦める。
                    continue;
                }
                prev = aStage.truck().pos();
                return a;
            }
        }
    }
}

//------------------------------------------------------------------------------
// EOF
