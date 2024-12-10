#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // sleep関数用
#include <string.h>

typedef struct {
    char name[50];
    int hp;
    int max_hp;
    int mp;
    int max_mp;
    int attack;
    int exp;
    int level;
} Character;

typedef struct {
    char item_name[50];
    int hp_heal;
    int mp_heal;
} Item;

typedef struct {
    char skill_name[50];
    int skill_cost;
    int damage;
    int hp_heal;
    int mp_heal;
} Skill;

void inputPlayerName(Character *player) {
    printf("名前を入力してください: ");
    scanf("%49s", player->name); // プレイヤーの名前を直接構造体に格納
}

int loadData(Character *player, Character *enemy) {
    FILE *file = fopen("task8.dat", "r");
    if (file == NULL) {
        printf("\033[31m保存データが見つかりません。\033[0m\n");
        return 0; // 読み込み失敗
    }

    if (fscanf(file, "%49s %d %d %d %d %d %d %d %d",
               player->name,
               &player->hp,
               &player->max_hp,
               &player->mp,
               &player->max_mp,
               &player->attack,
               &player->exp,
               &player->level,
               &enemy->level) != 9) {
        printf("\033[31mデータの読み込みに失敗しました。\033[0m\n");
        fclose(file);
        return 0; // 読み込み失敗
    }

    fclose(file);
    printf("\033[32mプレイヤーデータを読み込みました。\033[0m\n");
    return 1; // 読み込み成功
}


// ファイルにユーザーデータを書き込む
void saveData(Character *player, Character *enemy) {
    FILE *file = fopen("task8.dat", "w");
    if (file == NULL) {
        printf("ファイルを開けませんでした。\n");
        return;
    }
    fprintf(file, "%s\n", player->name);
    fprintf(file, "%d\n", player->hp);
    fprintf(file, "%d\n", player->max_hp);
    fprintf(file, "%d\n", player->mp);
    fprintf(file, "%d\n", player->max_mp);
    fprintf(file, "%d\n", player->attack);
    fprintf(file, "%d\n", player->exp);
    fprintf(file, "%d\n", player->level);
    fprintf(file, "%d\n", enemy->level);
    fclose(file);
    printf("データが保存されました。\n");
}

int getExpThreshold(int level) {
    return 100 + (level - 1) * 20;  // レベルが上がるごとに必要経験値が20増加
}

// 戦闘シーンの表示
void displayBattle(Character *player, Character *enemy) {
    int expToNextLevel = getExpThreshold(player->level);

    printf("\033[2J\033[H"); // 画面クリア
    printf("\033[32m【%s Lv.%d】\033[0m\n", player->name, player->level);
    printf("HP: %d/%d  MP: %d/%d  EXP: %d/%d\n", player->hp, player->max_hp, player->mp, player->max_mp, player->exp, expToNextLevel);

    printf("\033[31m【%s】\033[0m\n", enemy->name);
    printf("HP: %d\n", enemy->hp);
}

// 攻撃処理
void attack(Character *attacker, Character *defender) {
    int damage = rand() % attacker->attack + 3; // ランダムダメージ
    defender->hp -= damage;
    if (defender->hp < 0) defender->hp = 0;
    printf("\033[33m%s の攻撃！ %d ダメージ！\033[0m\n", attacker->name, damage);
    sleep(1);
}

// スキル処理
void useSkill(Character *player, Character *enemy, Skill *skill, int skillChoice) {
    int skillCost = skill->skill_cost;
    if (player->mp < skillCost) {
        printf("\033[31mMPが足りません！\033[0m\n");
        sleep(1);
        return;
    }
    if(skillChoice == 1 || skillChoice == 2){
        int damage = (rand() % skill->damage + 10);
        player->mp -= skillCost;
        enemy->hp -= damage;
        if (enemy->hp < 0) enemy->hp = 0;
        printf("\033[36m%s のスキル発動！ %d ダメージ！\033[0m\n", player->name, damage);
    }else if(skillChoice == 3){
        player->mp -= skillCost;
        player->hp += skill->hp_heal;
        if (player->hp > player->max_hp) player->hp = player->max_hp;
        printf("\033[36m%s の回復スキル発動！ HPが20回復した！\033[0m\n", player->name);
    }
    sleep(1);
}

// アイテム処理
void useItem(Character *player, Character *enemy, Item *item) {
    player->hp += item->hp_heal;
    player->mp += item->mp_heal;
    if (player->hp > player->max_hp) player->hp = player->max_hp;
    if (player->mp > player->max_mp) player->mp = player->max_mp;

    printf("\033[36m%s のアイテム発動！ HPが%d回復した！\033[0m\n", player->name, item->hp_heal);
    sleep(1);
}

// レベルアップ処理
void levelUp(Character *player) {
    player->level++;
    player->max_hp += 10;
    player->max_mp += 10;
    player->hp = player->max_hp;
    player->mp = player->max_mp;
    player->attack += 5;
    player->exp = 0;
    printf("\033[35mレベルアップ！レベル %d になった！HPとMPが全回復した！\033[0m\n", player->level);
    sleep(2);
}

// 敵のステータスを設定
void initEnemy(Character *enemy) {
    sprintf(enemy->name, "スライム Lv.%d", enemy->level);
    enemy->max_hp = 50 + (enemy->level * 5);
    enemy->hp = enemy->max_hp;
    enemy->attack = 15 + (enemy->level * 3);
}

// 敵の強さに応じた経験値を取得
int calculateExp(Character *enemy) {
    return (enemy->max_hp / 2) + (enemy->attack * 2);  // HPと攻撃力に応じて経験値が増加
}

int main() {
    srand(time(NULL));

    // プレイヤーの初期化
    Character player = {"プレイヤー", 100, 100, 50, 50, 20, 0, 1};
    Skill skill1 = {"スラッシュ", 10, 20, 0, 0};
    Skill skill2 = {"渾身斬り", 20, 30, 0, 0};
    Skill skill3 = {"ヒール", 10, 0, 30, 0};
    Skill skill[3] = {skill1, skill2, skill3};
    Character enemy;
    Item item1 = {"ポーション", 10, 20};
    char continueChoice;

    printf("\033[2J\033[H"); // 画面クリア
    printf("\033[34mターン制RPGゲームへようこそ！\033[0m\n\n");
    sleep(2);

    // 続きから始めるか、新規で始めるかの選択
    printf("1. 続きから始める\n");
    printf("2. 新規で始める\n");
    printf("選択: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        if (!loadData(&player, &enemy)) {
            printf("\033[31m新規ゲームを開始します。\033[0m\n");
            inputPlayerName(&player);
            sleep(1);
        }
    } else {
        printf("\033[32m新規ゲームを開始します。\033[0m\n");
        inputPlayerName(&player);
        sleep(1);
    }

    while (player.hp > 0) {
        // 新しい敵を初期化
        enemy.level++;
        initEnemy(&enemy);

        // 戦闘ループ
        while (player.hp > 0 && enemy.hp > 0) {
            displayBattle(&player, &enemy);

            // プレイヤーのターン
            printf("\033[32m【プレイヤーのターン】\033[0m\n");
            printf("1. 攻撃\n2. スキル\n3. アイテム\n選択: ");
            int choice;
            scanf("%d", &choice);

            if (choice == 2) {
                printf("\033[36mスキルを選択してください\033[0m\n");
                printf("1. スラッシュ(MP 10)\n2. 渾身斬り(MP 20)\n3. 回復(MP 20)\n選択: ");
                int skillChoice;
                scanf("%d", &skillChoice);
                if (skillChoice == 1) {
                    printf("\033[36m%sを選択しました\033[0m\n", skill[0].skill_name);
                    useSkill(&player, &enemy, &skill[0], skillChoice);
                } else if (skillChoice == 2) {
                    printf("\033[36m%sを選択しました\033[0m\n", skill[1].skill_name);
                    useSkill(&player, &enemy, &skill[1], skillChoice);
                } else if (skillChoice == 3) {
                    printf("\033[36m%sを選択しました\033[0m\n", skill[2].skill_name);
                    useSkill(&player, &enemy, &skill[2], skillChoice);
                } else {
                    printf("\033[31m無効な選択です\033[0m\n");
                    continue;
                }
                sleep(1);

            } else if (choice == 1) {
                attack(&player, &enemy);
            } else if (choice == 3) {
                printf("\033[36m所持アイテムから選択してください\033[0m\n");
                printf("1. ポーション(HP 10&MP 20回復)\n選択: ");
                int itemChoice;
                scanf("%d", &itemChoice);
                if (itemChoice == 1) {
                    printf("\033[36m%sを選択しました\033[0m\n", item1.item_name);
                    useItem(&player, &enemy, &item1);
                } else {
                    printf("\033[31m無効な選択です\033[0m\n");
                    continue;
                }
            } else {
                printf("\033[31m無効な選択です\033[0m\n");
                continue;
            }

            if (enemy.hp == 0) break;

            // 敵のターン
            displayBattle(&player, &enemy);
            printf("\033[31m【敵のターン】\033[0m\n");
            attack(&enemy, &player);
        }

        // 勝敗判定
        displayBattle(&player, &enemy);
        if (player.hp > 0) {
            int gainedExp = calculateExp(&enemy);
            printf("\033[32mおめでとう！敵を倒した！\033[0m\n");
            printf("\033[33m経験値 %d 獲得！\033[0m\n", gainedExp);
            player.exp += gainedExp;

            // レベルアップ判定
            while (player.exp >= getExpThreshold(player.level)) {
                levelUp(&player);
            }
        } else {
            printf("\033[31mゲームオーバー...\033[0m\n");
            printf("\nゲームを続けますか？(y/n): ");
            scanf(" %c", &continueChoice);
            if (continueChoice != 'y' && continueChoice != 'Y') {
                printf("\033[34mゲームを終了します。\033[0m\n");
                break;
            }
        }

        // データをファイルに保存
        saveData(&player, &enemy);

        // 次の戦闘に進むか確認
        printf("\n次の敵と戦いますか？ (y/n): ");
        scanf(" %c", &continueChoice);
        if (continueChoice != 'y' && continueChoice != 'Y') {
            printf("\033[34mゲームを終了します。\033[0m\n");
            break;
        }
    }

    return 0;
}
