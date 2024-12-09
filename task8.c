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

int loadData(Character *player) {
    FILE *file = fopen("task8.dat", "r");
    if (file == NULL) {
        printf("\033[31m保存データが見つかりません。\033[0m\n");
        return 0; // 読み込み失敗
    }

    if (fscanf(file, "%49s %d %d %d %d %d %d %d",
               player->name,
               &player->hp,
               &player->max_hp,
               &player->mp,
               &player->max_mp,
               &player->attack,
               &player->exp,
               &player->level) != 8) {
        printf("\033[31mデータの読み込みに失敗しました。\033[0m\n");
        fclose(file);
        return 0; // 読み込み失敗
    }

    fclose(file);
    printf("\033[32mプレイヤーデータを読み込みました。\033[0m\n");
    return 1; // 読み込み成功
}


// ファイルにユーザーデータを書き込む
void saveData(Character *player) {
    FILE *file = fopen("task8.dat", "w");
    if (file == NULL) {
        printf("ファイルを開けませんでした。\n");
        return;
    }
    fprintf(file, "名前: %s\n", player->name);
    fprintf(file, "レベル: %d\n", player->level);
    fprintf(file, "HP: %d/%d\n", player->hp, player->max_hp);
    fprintf(file, "MP: %d/%d\n", player->mp, player->max_mp);
    fprintf(file, "経験値: %d\n", player->exp);
    fclose(file);
    printf("データが保存されました。\n");
}

// 戦闘シーンの表示
void displayBattle(Character *player, Character *enemy) {
    printf("\033[2J\033[H"); // 画面クリア
    printf("\033[32m【%s】\033[0m\nHP: %d/%d  MP: %d/%d\n", player->name, player->hp, player->max_hp, player->mp, player->max_mp);
    printf("\033[31m【%s】\033[0m\nHP: %d\n", enemy->name, enemy->hp);
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
void useSkill(Character *player, Character *enemy) {
    const int skillCost = 10;
    if (player->mp < skillCost) {
        printf("\033[31mMPが足りません！\033[0m\n");
        sleep(1);
        return;
    }
    int damage = (rand() % player->attack + 1) * 1.5; // スキルは攻撃の2倍の威力
    player->mp -= skillCost;
    enemy->hp -= damage;
    if (enemy->hp < 0) enemy->hp = 0;
    printf("\033[36m%s のスキル発動！ %d ダメージ！\033[0m\n", player->name, damage);
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
void initEnemy(Character *enemy, int battleCount) {
    sprintf(enemy->name, "敵スライム Lv.%d", battleCount);
    enemy->max_hp = 50 + (battleCount * 5); // 戦闘回数に応じてHP増加
    enemy->hp = enemy->max_hp;
    enemy->attack = 13 + (battleCount * 2);  // 戦闘回数に応じて攻撃力増加
}

int getExpThreshold(int level) {
    return 100 + (level - 1) * 20;  // レベルが上がるごとに必要経験値が20増加
}

// 敵の強さに応じた経験値を取得
int calculateExp(Character *enemy) {
    return (enemy->max_hp / 2) + (enemy->attack * 2);  // HPと攻撃力に応じて経験値が増加
}

int main() {
    srand(time(NULL));

    // プレイヤーの初期化
    Character player = {"プレイヤー", 100, 100, 50, 50, 20, 0, 1};

    int battleCount = 0;
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
        if (!loadData(&player)) {
            printf("\033[31m新規ゲームを開始します。\033[0m\n");
            sleep(1);
        }
    } else {
        printf("\033[32m新規ゲームを開始します。\033[0m\n");
        sleep(1);
    }

    while (player.hp > 0) {
        // 新しい敵を初期化
        Character enemy;
        battleCount++;
        initEnemy(&enemy, battleCount);

        // 戦闘ループ
        while (player.hp > 0 && enemy.hp > 0) {
            displayBattle(&player, &enemy);

            // プレイヤーのターン
            printf("\033[32m【プレイヤーのターン】\033[0m\n");
            printf("1. 攻撃\n2. スキル (MP 10 消費)\n選択: ");
            int choice;
            scanf("%d", &choice);

            if (choice == 2) {
                useSkill(&player, &enemy);
            } else {
                attack(&player, &enemy);
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
            saveData(&player);
            printf("\nゲームを続けますか？(y/n): ");
            scanf(" %c", &continueChoice);
            if (continueChoice != 'y' && continueChoice != 'Y') {
                printf("\033[34mゲームを終了します。\033[0m\n");
                break;
            }
        }

        // データをファイルに保存
        saveData(&player);

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
