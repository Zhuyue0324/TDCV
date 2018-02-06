...build your model...
# Add the optimizer
train_op = tf.train.AdamOptimizer(1e-4).minimize(cross_entropy)
# Add the ops to initialize variables.  These will include
# the optimizer slots added by AdamOptimizer().
init_op = tf.initialize_all_variables()

# launch the graph in a session
sess = tf.Session()
# Actually intialize the variables
sess.run(init_op)
# now train your model
for ...:
  sess.run(train_op)