#include "EnemyData.h"

//AND演算
EnemyStatus operator&(EnemyStatus status1,EnemyStatus status2) {
	uint32_t statusNum1 = static_cast<uint32_t>(status1);
	uint32_t statusNum2 = static_cast<uint32_t>(status2);

	uint32_t result = statusNum1 & statusNum2;
	return static_cast<EnemyStatus>(result);
}

//OR演算
EnemyStatus operator|(EnemyStatus status1,EnemyStatus status2) {
	uint32_t statusNum1 = static_cast<uint32_t>(status1);
	uint32_t statusNum2 = static_cast<uint32_t>(status2);

	uint32_t result = statusNum1 | statusNum2;
	return static_cast<EnemyStatus>(result);
}

//反転
EnemyStatus operator~(EnemyStatus status) {
	uint32_t statusNum = static_cast<uint32_t>(status);

	uint32_t result = ~statusNum;
	return static_cast<EnemyStatus>(result);
}
