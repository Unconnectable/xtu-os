def opt_page_faults(ref_seq, frame_capacity):
    frames = []
    page_faults = 0
    
    for i, page in enumerate(ref_seq):
        if page not in frames:
            if len(frames) < frame_capacity:
                frames.append(page)
            else:
                # 找到未来最远使用的页面
                future_uses = {}
                for frame_page in frames:
                    try:
                        future_uses[frame_page] = ref_seq[i+1:].index(frame_page) + i + 1
                    except ValueError:
                        future_uses[frame_page] = float('inf')
                
                # 替换未来最远使用的页面
                page_to_replace = max(future_uses, key=future_uses.get)
                frames[frames.index(page_to_replace)] = page
            page_faults += 1
    
    return page_faults
def opt_page_replacements(ref_seq, frame_capacity):
    frames = []
    page_faults = 0
    replacements = 0  # 新增:记录置换次数
    
    for i, page in enumerate(ref_seq):
        if page not in frames:
            if len(frames) < frame_capacity:
                frames.append(page)
            else:
                    future_uses = {}
                    for frame_page in frames:
                        try:
                            future_uses[frame_page] = ref_seq[i+1:].index(frame_page) + i + 1
                        except ValueError:
                            future_uses[frame_page] = float('inf')
                    
                    # 找到未来最远使用的页面并替换
                    page_to_replace = max(future_uses, key=future_uses.get)
                    frames[frames.index(page_to_replace)] = page
                    replacements += 1  # 置换次数加一
                
            page_faults += 1
    
    return replacements

from collections import deque

def fifo_page_replacements(ref_seq, frame_capacity):
    frames = set()         # 当前内存中的页面集合(用于快速查找)
    queue = deque()        # 用于记录页面进入内存的顺序
    replacements = 0       # 页面置换次数
    page_faults = 0        # 可选:缺页次数
    
    for page in ref_seq:
        if page not in frames:
            page_faults += 1
            
            if len(frames) < frame_capacity:
                frames.add(page)
                queue.append(page)
            else:
                # 替换最早进入的页面
                oldest_page = queue.popleft()
                frames.remove(oldest_page)
                frames.add(page)
                queue.append(page)
                replacements += 1  # 增加置换次数
                
    return replacements


def lfu_page_stats(ref_seq, frame_capacity):
    frames = {}               # 当前内存中的页面 {page: True}
    page_freq = {}            # 每个页面的访问频率 {page: frequency}
    page_faults = 0           # 缺页次数
    replacements = 0          # 页面置换次数

    for page in ref_seq:
        # 更新该页面的访问频率(无论是否在内存中)
        if page in page_freq:
            page_freq[page] += 1
        else:
            page_freq[page] = 1

        if page in frames:
            # 页面已在内存中,不发生缺页
            pass
        else:
            # 发生缺页
            page_faults += 1

            if len(frames) < frame_capacity:
                # 内存未满,直接加入
                frames[page] = True
            else:
                # 内存已满,需要进行页面置换
                # 找出当前内存中访问频率最低的页面
                min_freq_page = min(frames.keys(), key=lambda p: page_freq[p])
                del frames[min_freq_page]
                frames[page] = True
                replacements += 1  # 置换次数加一

    return page_faults, replacements


def lru2_page_stats(ref_seq, frame_capacity):
    """
    模拟 LRU-2 页面置换算法并计算缺页和换页次数.

    LRU-2 页面置换算法(Least Recently Used 2nd Chance)是一种改进的 LRU 算法.
    它跟踪每个页面的最近两次访问时间.当需要置换页面时,它优先置换
    其第二次最近访问时间最久的页面.如果存在多个这样的页面,则选择其
    最近一次访问时间最久的页面.

    Args:
        ref_seq (list): 页面访问序列.
        frame_capacity (int): 内存帧的容量.

    Returns:
        tuple: (缺页次数, 换页次数).
               缺页次数是指访问的页面不在内存中的次数.
               换页次数是指需要将页面从内存中换出的次数.
    """
    memory = {}  # 存储内存中的页面,键为页面号,值为一个列表 [最近一次访问时间, 第二次最近访问时间]
    page_faults = 0
    page_replacements = 0
    time = 0  # 模拟时间戳

    for page in ref_seq:
        time += 1

        if page in memory:
            # 页面在内存中,更新访问时间
            memory[page][1] = memory[page][0]  # 第二次最近访问时间更新为之前的最近一次访问时间
            memory[page][0] = time            # 最近一次访问时间更新为当前时间
        else:
            # 页面不在内存中,发生缺页
            page_faults += 1

            if len(memory) < frame_capacity:
                # 内存未满,直接加载页面
                memory[page] = [time, 0]  # 第一次访问,第二次最近访问时间设为0
            else:
                # 内存已满,需要进行页面置换
                page_replacements += 1

                # 查找要置换的页面
                page_to_replace = None
                min_second_recent_time = float('inf')
                min_recent_time = float('inf')

                for p, times in memory.items():
                    if times[1] < min_second_recent_time:
                        min_second_recent_time = times[1]
                        min_recent_time = times[0]
                        page_to_replace = p
                    elif times[1] == min_second_recent_time:
                        # 第二次最近访问时间相同,比较最近一次访问时间
                        if times[0] < min_recent_time:
                            min_recent_time = times[0]
                            page_to_replace = p

                # 将要置换的页面从内存中移除
                del memory[page_to_replace]

                # 将新页面加载到内存中
                memory[page] = [time, 0]

    return page_faults, page_replacements
ref_seq = [    31, 20, 25, 61, 90, 38, 67, 32, 52, 72, 83, 78, 51, 24, 97, 77, 10,
    0,  20, 48, 75, 48, 44, 99, 35, 53, 3,  24, 24, 10, 79, 7,  60, 76,
    84, 35, 23, 35, 11, 58, 67, 4,  50, 29, 31, 52, 95, 21, 92, 48, 4,
    57, 31, 62, 72, 82, 90, 98, 63, 3,  85, 89, 3,  9,  62, 77, 65, 48,
    51, 7,  31, 1,  10, 46, 83, 2,  48, 52, 98, 5,  95, 54, 48, 36, 37,
    10, 31, 11, 45, 44, 81, 53, 74, 33, 64, 18, 20, 73, 58, 79]
frame_capacity = 3

print("缺页 换页", lru2_page_stats(ref_seq, frame_capacity))