import typing
import numpy as np

"""
两遍扫描法 获取连通域
"""

demo_list = [
    0, 0, 1, 0, 0, 1, 0,
    1, 1, 1, 0, 1, 1, 1,
    0, 0, 1, 0, 0, 1, 0,
    0, 1, 1, 0, 1, 1, 0,
]


def two_pass(array: np.ndarray) -> np.ndarray:
    row, column = array.shape
    id_array: np.ndarray = np.zeros((row, column), dtype=int)
    id_adjacent: typing.Dict[int, typing.Set[int]] = {}
    _id: int = 1

    for i in range(row):
        for j in range(column):
            if i == 0 and j == 0:
                id_array[i][j] = _id
                id_adjacent[_id] = set()
                _id += 1
            elif i == 0:
                if array[i][j] == array[i][j-1]:
                    id_array[i][j] = id_array[i][j-1]
                else:
                    id_array[i][j] = _id
                    id_adjacent[_id] = set()
                    _id += 1
            elif j == 0:
                if array[i][j] == array[i-1][j]:
                    id_array[i][j] = id_array[i-1][j]
                else:
                    id_array[i][j] = _id
                    id_adjacent[_id] = set()
                    _id += 1
            else:
                if array[i][j] == array[i-1][j] and array[i][j] != array[i][j-1]:
                    id_array[i][j] = id_array[i-1][j]
                elif array[i][j] == array[i][j-1] and array[i][j] != array[i-1][j]:
                    id_array[i][j] = id_array[i][j-1]
                elif array[i][j] != array[i][j-1] and array[i][j] != array[i-1][j]:
                    id_array[i][j] = _id
                    id_adjacent[_id] = set()
                    _id += 1
                else:
                    id_array[i][j] = id_array[i-1][j]
                    if id_array[i][j-1] != id_array[i-1][j]:
                        id_adjacent[id_array[i-1][j]].add(id_array[i][j-1])
    for i in range(_id-1, 0, -1):
        merge = False
        for j in range(i-1, 0, -1):
            if i in id_adjacent[j] or not id_adjacent[j].isdisjoint(id_adjacent[i]):
                id_adjacent[j].update(id_adjacent[i])
                merge = True
        if merge:
            del id_adjacent[i]
    for i in range(row):
        for j in range(column):
            if id_array[i][j] in id_adjacent:
                continue
            for k in id_adjacent:
                if id_array[i][j] in id_adjacent[k]:
                    id_array[i][j] = k
                    break
    return id_array


if __name__ == "__main__":
    print(two_pass(np.array(demo_list).reshape(4, 7)))
