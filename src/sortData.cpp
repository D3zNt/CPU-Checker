#include <sortData.hpp>

void mergin(std::vector<CPU_DATA>& vec, int left, int mid, int right) {
    std::vector<CPU_DATA> leftVec(vec.begin() + left, vec.begin() + mid + 1);
    std::vector<CPU_DATA> rightVec(vec.begin() + mid + 1, vec.begin() + right + 1);

    int i = 0, j = 0, k = left;

    while (i < leftVec.size() && j < rightVec.size()) {
        if (leftVec[i].cpu > rightVec[j].cpu) {
            vec[k++] = leftVec[i++];
        } else {
            vec[k++] = rightVec[j++];
        }
    }

    while (i < leftVec.size()) vec[k++] = leftVec[i++];
    while (j < rightVec.size()) vec[k++] = rightVec[j++];
}

void sortmerger(std::vector<CPU_DATA>& vec, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        sortmerger(vec, left, mid);
        sortmerger(vec, mid + 1, right);
        mergin(vec, left, mid, right);
    }
}