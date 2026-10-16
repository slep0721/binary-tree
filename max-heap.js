class MaxHeap {
  constructor(values = []) {
    this.values = [];
    for (const value of values) this.insert(value);
  }

  get size() {
    return this.values.length;
  }

  peek() {
    return this.values[0];
  }

  insert(value) {
    if (typeof value !== 'number' || !Number.isFinite(value)) {
      throw new TypeError('MaxHeap only accepts finite numbers.');
    }

    this.values.push(value);
    this.#siftUp(this.values.length - 1);
    return this;
  }

  extractMax() {
    if (this.values.length === 0) return undefined;
    if (this.values.length === 1) return this.values.pop();

    const maximum = this.values[0];
    this.values[0] = this.values.pop();
    this.#siftDown(0);
    return maximum;
  }

  toArray() {
    return [...this.values];
  }

  #siftUp(index) {
    while (index > 0) {
      const parent = Math.floor((index - 1) / 2);
      if (this.values[parent] >= this.values[index]) break;
      [this.values[parent], this.values[index]] = [this.values[index], this.values[parent]];
      index = parent;
    }
  }

  #siftDown(index) {
    while (true) {
      const left = index * 2 + 1;
      const right = left + 1;
      let largest = index;

      if (left < this.values.length && this.values[left] > this.values[largest]) largest = left;
      if (right < this.values.length && this.values[right] > this.values[largest]) largest = right;
      if (largest === index) break;

      [this.values[index], this.values[largest]] = [this.values[largest], this.values[index]];
      index = largest;
    }
  }
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = MaxHeap;
}
