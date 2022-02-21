package parallelTree;

public class TreeNode {
	int name;
	public TreeNode left = null;
	public TreeNode right = null;

	public TreeNode(int name) {
		this.name = name;
	}

	public synchronized void addChild(TreeNode child) {
		if (left == null) {
			left = child;
			return;
		}
		right = child;
	}

	public TreeNode getNode(int name) {
		boolean hasLeft;
		boolean hasRight;
		synchronized (this) {
			hasLeft = left != null;
			hasRight = right != null;
		}

		TreeNode aux = null;
		if (this.name == name) {
			return this;
		}

		if (hasLeft) {
			aux = left.getNode(name);
		}

		if (aux != null) {
			return aux;
		}

		if (hasRight) {
			aux = right.getNode(name);
		}

		return aux;
	}
}
