#include <string>
#include <vector>

class ScriptEdit
{
public:
	int scriptIndex = -1; //The index of the associated script in the main table
	int editType = 0; //What to replace the data with 1 = Type, 2 = Flag, 3 = ObjectID
	int associatedOffset = 0; //The offset from the start of the file this data is located;
	int rewardIndex = 0; //Which reward the edit applies to
public:ScriptEdit(int newScriptIndex, int newEditType, int newAssociatedOffset, int newRewardIndex)
{
	this->scriptIndex = newScriptIndex;
	this->editType = newEditType;
	this->associatedOffset = newAssociatedOffset;
	this->rewardIndex = newRewardIndex;
}
};